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

template <typename T>
void Set(KuuraHandle* handle, void(Kuura::IWorkload::* setter)(T), T value)
{
	assert(handle);
	assert(setter);
	assert(value);

	const auto instance = reinterpret_cast<Kuura::IWorkload*>(handle);

	if (instance)
	{
		(instance->*setter)(value);
	}
}

KuuraHandle* KUURA_CALLING_CONVENTION KuuraInitialize(
	const char* path,
	KuuraTargetType type,
	bool removeAfterWipe,
	void* context)
{
	if (!path)
	{
		return nullptr;
	}

	switch (type)
	{
		case KuuraTargetType::FileWipe:
			return reinterpret_cast<KuuraHandle*>(
				new Kuura::FileWipe(path, removeAfterWipe, context));

		case KuuraTargetType::DirectoryWipe:
			return reinterpret_cast<KuuraHandle*>(
				new Kuura::DirectoryWipe(path, removeAfterWipe, context));

		case KuuraTargetType::DriveWipe:
			if (!removeAfterWipe)
			{
				return reinterpret_cast<KuuraHandle*>(new Kuura::DriveWipe(path, context));
			}
	}

	return nullptr;
}

bool KUURA_CALLING_CONVENTION KuuraAddWipeRound(
	struct KuuraHandle* handle,
	KuuraFillType round,
	bool verify,
	const char* optional)
{
	const auto instance = reinterpret_cast<Kuura::IWorkload*>(handle);

	if (!instance)
	{
		return false;
	}

	switch (round)
	{
		case KuuraFillType::ZeroFill:
			instance->AddFiller(new Kuura::ByteFiller(std::byte(0x00), verify));
			return true;

		case KuuraFillType::OneFill:
			instance->AddFiller(new Kuura::ByteFiller(std::byte(0xFF), verify));
			return true;

		case KuuraFillType::ByteFill:
			if (Length(optional) != 1)
			{
				return false;
			}

			instance->AddFiller(new Kuura::ByteFiller(std::byte(optional[0]), verify));
			return true;

		case KuuraFillType::SequenceFill:
			if (Length(optional) < 1)
			{
				return false;
			}

			instance->AddFiller(new Kuura::SequenceFiller(optional, verify));
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
			instance->AddFiller(new Kuura::RandomFiller(verify));
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
	const auto operation = reinterpret_cast<Kuura::IWorkload*>(handle);

	if (!operation)
	{
		return false;
	}

	return operation->Run();
}

void KUURA_CALLING_CONVENTION KuuraFree(KuuraHandle* handle)
{
	if (handle)
	{
		delete reinterpret_cast<Kuura::IWorkload*>(handle);
	}
}
