#include "KuuraAPI-PCH.hpp"
#include "KuuraAPI.h"

#include "Workloads/KuuraWorkload.hpp"
#include "Workloads/KuuraFileWipe.hpp"
#include "Workloads/KuuraDirectoryWipe.hpp"
#include "Workloads/KuuraDriveWipe.hpp"

#include "FillProviders/KuuraFillProvider.hpp"
#include "FillProviders/KuuraByteFiller.hpp"
#include "FillProviders/KuuraSequenceFiller.hpp"
#include "FillProviders/KuuraFileFiller.hpp"
#include "FillProviders/KuuraRandomFiller.hpp"

#include "KuuraDataUnits.hpp"

struct KuuraObject
{
	KuuraObject(void* context) :
		Context(context)
	{
	}

	void AddWorkload(std::shared_ptr<Kuura::IWorkload>&& workload)
	{
		Workloads.emplace_back(workload);
	}

	void AddFiller(std::shared_ptr<Kuura::IFillProvider>&& filler)
	{
		for (auto& workload : Workloads)
		{
			workload->AddFiller(filler);
		}
	}

	bool Run()
	{
		for (auto& workload : Workloads)
		{
			if (!workload->Run())
			{
				return false;
			}
		}

		return true;
	}

	void* Context = nullptr;
	std::vector<std::shared_ptr<Kuura::IWorkload>> Workloads;
};

constexpr size_t Length(const char* str)
{
	char* ptr = const_cast<char*>(str);

	if (ptr)
	{
		while (*ptr)
		{
			ptr++;
		}
	}

	return (ptr - str);
}

template <typename T>
void Set(KuuraHandle* handle, void(Kuura::IWorkload::* setter)(T), T value)
{
	assert(handle);
	assert(setter);
	assert(value);

	auto instance = reinterpret_cast<KuuraObject*>(handle);

	if (!instance)
	{
		return;
	}

	for (auto& workload : instance->Workloads)
	{
		(workload.get()->*setter)(value);
	}
}

KuuraHandle* KUURA_CALLING_CONVENTION KuuraInitialize(
	void* context)
{
	return reinterpret_cast<KuuraHandle*>(new KuuraObject(context));
}

bool KUURA_CALLING_CONVENTION KuuraAddTarget(
	struct KuuraHandle* handle,
	const char* path,
	enum KuuraTargetType type,
	bool removeAfterWipe)
{
	if (!path)
	{
		return false;
	}

	auto kuuraObject = reinterpret_cast<KuuraObject*>(handle);

	if (!kuuraObject)
	{
		return false;
	}

	switch (type)
	{
		case KuuraTargetType::FileWipe:
		{
			kuuraObject->AddWorkload(std::make_shared<Kuura::FileWipe>(path, removeAfterWipe, kuuraObject->Context));
			return true;
		}
		case KuuraTargetType::DirectoryWipe:
		{
			kuuraObject->AddWorkload(std::make_shared<Kuura::DirectoryWipe>(path, removeAfterWipe, kuuraObject->Context));
			return true;
		}
		case KuuraTargetType::DriveWipe:
		{
			if (removeAfterWipe)
			{
				return false;
			}

			kuuraObject->AddWorkload(std::make_shared<Kuura::DriveWipe>(path, kuuraObject->Context));
			return true;
		}
	}

	return false;
}

bool KUURA_CALLING_CONVENTION KuuraAddWipeRound(
	struct KuuraHandle* handle,
	KuuraFillType round,
	bool verify,
	const char* optional)
{
	const auto instance = reinterpret_cast<KuuraObject*>(handle);

	if (!instance)
	{
		return false;
	}

	switch (round)
	{
		case KuuraFillType::ZeroFill:
			instance->AddFiller(std::make_shared<Kuura::ByteFiller>(std::byte(0x00), verify));
			return true;

		case KuuraFillType::OneFill:
			instance->AddFiller(std::make_shared<Kuura::ByteFiller>(std::byte(0xFF), verify));
			return true;

		case KuuraFillType::ByteFill:
			if (Length(optional) != 1)
			{
				return false;
			}

			instance->AddFiller(std::make_shared<Kuura::ByteFiller>(std::byte(optional[0]), verify));
			return true;

		case KuuraFillType::SequenceFill:
			if (Length(optional) < 1)
			{
				return false;
			}

			instance->AddFiller(std::make_shared<Kuura::SequenceFiller>(optional, verify));
			return true;

		case KuuraFillType::FileFill:
			if (Length(optional) < 1)
			{
				return false;
			}
			{
				auto fileFiller = std::make_shared<Kuura::FileFiller>(optional, verify);

				if (fileFiller->HasContent())
				{
					instance->AddFiller(fileFiller);
					return true;
				}
			}
			return false;

		case KuuraFillType::RandomFill:
			instance->AddFiller(std::make_shared<Kuura::RandomFiller>(verify));
			return true;

	}

	return false;
}

void KUURA_CALLING_CONVENTION KuuraSetWipeStartedCallback(
	KuuraHandle* handle,
	KuuraWipeStartedCallback* callback)
{
	Set(handle, &Kuura::IWorkload::SetWipeStartedCallback, callback);
}

void KUURA_CALLING_CONVENTION KuuraSetPassStartedCallback(
	KuuraHandle* handle,
	KuuraPassStartedCallback* callback)
{
	Set(handle, &Kuura::IWorkload::SetPassStartedCallback, callback);
}

void KUURA_CALLING_CONVENTION KuuraSetProgressCallback(
	KuuraHandle* handle,
	KuuraProgressCallback* callback)
{
	Set(handle, &Kuura::IWorkload::SetProgressCallback, callback);
}

void KUURA_CALLING_CONVENTION KuuraSetPassCompletedCallback(
	KuuraHandle* handle,
	KuuraPassCompletedCallback* callback)
{
	Set(handle, &Kuura::IWorkload::SetPassCompletedCallback, callback);
}

void KUURA_CALLING_CONVENTION KuuraSetWipeCompletedCallback(
	KuuraHandle* handle,
	KuuraWipeCompletedCallback* callback)
{
	Set(handle, &Kuura::IWorkload::SetWipeCompletedCallback, callback);
}

void KUURA_CALLING_CONVENTION KuuraSetErrorCallback(
	KuuraHandle* handle,
	KuuraErrorCallback* callback)
{
	Set(handle, &Kuura::IWorkload::SetErrorCallback, callback);
}

bool KUURA_CALLING_CONVENTION KuuraRun(KuuraHandle* handle)
{
	const auto instance = reinterpret_cast<KuuraObject*>(handle);

	if (!instance)
	{
		return false;
	}

	return instance->Run();
}

void KUURA_CALLING_CONVENTION KuuraFree(KuuraHandle* handle)
{
	if (handle)
	{
		delete reinterpret_cast<KuuraObject*>(handle);
	}
}
