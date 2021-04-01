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

class KuuraObject
{
public:
	KuuraObject(void* context) :
		Callbacks(context)
	{
	}

	bool AddWorkload(const char* path, KuuraTargetType type, bool removeAfterWipe)
	{
		switch (type)
		{
			case KuuraTargetType::FileWipe:
			{
				m_workloads.emplace_back(new Kuura::FileWipe(Callbacks, path, removeAfterWipe));
				return true;
			}
			case KuuraTargetType::DirectoryWipe:
			{
				m_workloads.emplace_back(new Kuura::DirectoryWipe(Callbacks, path, removeAfterWipe));
				return true;
			}
			case KuuraTargetType::DriveWipe:
			{
				if (removeAfterWipe)
				{
					return false;
				}

				m_workloads.emplace_back(new Kuura::DriveWipe(Callbacks, path));
				return true;
			}
		}

		return false;
	}

	bool AddFiller(KuuraFillType type, bool verify, const char* optional)
	{
		switch (type)
		{
			case KuuraFillType::ZeroFill:
			{
				m_fillers.emplace_back(new Kuura::ByteFiller(std::byte(0x00), verify));
				return true;
			}
			case KuuraFillType::OneFill:
			{
				m_fillers.emplace_back(new Kuura::ByteFiller(std::byte(0xFF), verify));
				return true;
			}
			case KuuraFillType::ByteFill:
			{
				if (Length(optional) != 1)
				{
					return false;
				}

				m_fillers.emplace_back(new Kuura::ByteFiller(std::byte(optional[0]), verify));
				return true;
			}
			case KuuraFillType::SequenceFill:
			{
				if (Length(optional) < 1)
				{
					return false;
				}

				m_fillers.emplace_back(new Kuura::SequenceFiller(optional, verify));
				return true;
			}
			case KuuraFillType::FileFill:
			{
				if (Length(optional) < 1)
				{
					return false;
				}
					
				auto fileFiller = std::make_shared<Kuura::FileFiller>(optional, verify);

				if (!fileFiller->HasContent())
				{
					return false;
				}

				m_fillers.emplace_back(fileFiller);
				return true;
			}
			case KuuraFillType::RandomFill:
			{
				m_fillers.emplace_back(new Kuura::RandomFiller(verify));
				return true;
			}
		}

		return false;
	}

	bool Run()
	{
		// TODO: this is shit
		for (auto& workload : m_workloads)
		{
			for (auto& filler : m_fillers)
			{
				workload->AddFiller(filler);
			}

			if (!workload->Run())
			{
				return false;
			}
		}

		return true;
	}

	Kuura::CallbackContainer Callbacks;

private:
	std::vector<std::shared_ptr<Kuura::IWorkload>> m_workloads;
	std::vector<std::shared_ptr<Kuura::IFillProvider>> m_fillers;
};

template <typename T>
void Set(KuuraHandle* handle, T(Kuura::CallbackContainer::* field), T value)
{
	assert(handle);
	assert(field);
	assert(value);

	auto instance = reinterpret_cast<KuuraObject*>(handle);

	if (!instance)
	{
		return;
	}

	instance->Callbacks.*field = value;
}

KuuraHandle* KUURA_CALLING_CONVENTION KuuraInitialize(void* context)
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

	return kuuraObject->AddWorkload(path, type, removeAfterWipe);
}

bool KUURA_CALLING_CONVENTION KuuraAddWipeRound(
	struct KuuraHandle* handle,
	KuuraFillType type,
	bool verify,
	const char* optional)
{
	const auto instance = reinterpret_cast<KuuraObject*>(handle);

	if (!instance)
	{
		return false;
	}

	return instance->AddFiller(type, verify, optional);
}

void KUURA_CALLING_CONVENTION KuuraSetWipeStartedCallback(
	KuuraHandle* handle,
	KuuraWipeStartedCallback* callback)
{
	Set(handle, &Kuura::CallbackContainer::WipeStartedCallback, callback);
}

void KUURA_CALLING_CONVENTION KuuraSetPassStartedCallback(
	KuuraHandle* handle,
	KuuraPassStartedCallback* callback)
{
	Set(handle, &Kuura::CallbackContainer::PassStartedCallback, callback);
}

void KUURA_CALLING_CONVENTION KuuraSetProgressCallback(
	KuuraHandle* handle,
	KuuraProgressCallback* callback)
{
	Set(handle, &Kuura::CallbackContainer::ProgressCallback, callback);
}

void KUURA_CALLING_CONVENTION KuuraSetPassCompletedCallback(
	KuuraHandle* handle,
	KuuraPassCompletedCallback* callback)
{
	Set(handle, &Kuura::CallbackContainer::PassCompletedCallback, callback);
}

void KUURA_CALLING_CONVENTION KuuraSetWipeCompletedCallback(
	KuuraHandle* handle,
	KuuraWipeCompletedCallback* callback)
{
	Set(handle, &Kuura::CallbackContainer::WipeCompletedCallback, callback);
}

void KUURA_CALLING_CONVENTION KuuraSetErrorCallback(
	KuuraHandle* handle,
	KuuraErrorCallback* callback)
{
	Set(handle, &Kuura::CallbackContainer::ErrorCallback, callback);
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
