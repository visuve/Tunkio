#include "TunkioAPI-PCH.hpp"
#include "TunkioAPI.h"

#include "Workloads/TunkioWorkload.hpp"
#include "Workloads/TunkioFileWipe.hpp"
#include "Workloads/TunkioDirectoryWipe.hpp"
#include "Workloads/TunkioDriveWipe.hpp"

#include "FillProviders/TunkioFillProvider.hpp"
#include "FillProviders/TunkioCharFiller.hpp"
#include "FillProviders/TunkioSentenceFiller.hpp"
#include "FillProviders/TunkioFileFiller.hpp"
#include "FillProviders/TunkioRandomFiller.hpp"

#include "TunkioDataUnits.hpp"

template <typename T>
void Set(TunkioHandle* handle, void(Tunkio::IWorkload::*setter)(T), T value)
{
	assert(handle);
	assert(setter);
	assert(value);

	const auto instance = reinterpret_cast<Tunkio::IWorkload*>(handle);

	if (instance)
	{
		(instance->*setter)(value);
	}
}

TunkioHandle* TUNKIO_CALLING_CONVENTION TunkioInitialize(
	const char* path,
	TunkioTargetType type,
	bool removeAfterWipe,
	void* context)
{
	if (!path)
	{
		return nullptr;
	}

	// TODO: here we need to check the physical sector size to find the optimal buffer size

	switch (type)
	{
		case TunkioTargetType::FileWipe:
			return reinterpret_cast<TunkioHandle*>(new Tunkio::FileWipe(path, removeAfterWipe, context));
		case TunkioTargetType::DirectoryWipe:
			return reinterpret_cast<TunkioHandle*>(new Tunkio::DirectoryWipe(path, removeAfterWipe, context));
		case TunkioTargetType::DriveWipe:
			if (!removeAfterWipe)
			{
				return reinterpret_cast<TunkioHandle*>(new Tunkio::DriveWipe(path, context));
			}
	}

	return nullptr;
}

bool TUNKIO_CALLING_CONVENTION TunkioAddWipeRound(
	struct TunkioHandle* handle,
	TunkioFillType round,
	bool verify,
	const char* optional)
{
	const auto instance = reinterpret_cast<Tunkio::IWorkload*>(handle);

	if (!instance)
	{
		return false;
	}

	constexpr Tunkio::DataUnit::Mebibytes bufferSize(1);

	switch (round)
	{
		case TunkioFillType::ZeroFill:
			instance->AddFiller(new Tunkio::CharFiller(bufferSize, 0x00, verify));
			return true;

		case TunkioFillType::OneFill:
			instance->AddFiller(new Tunkio::CharFiller(bufferSize, 0xFF, verify));
			return true;

		case TunkioFillType::CharacterFill:
			if (!optional || strlen(optional) != 1)
			{
				return false;
			}

			instance->AddFiller(new Tunkio::CharFiller(bufferSize, optional[0], verify));
			return true;

		case TunkioFillType::SentenceFill:
			if (!optional || !strlen(optional))
			{
				return false;
			}

			instance->AddFiller(new Tunkio::SentenceFiller(bufferSize, optional, verify));
			return true;

		case TunkioFillType::FileFill:
			if (!optional || !strlen(optional))
			{
				return false;
			}
			{
				auto fileFiller = std::make_shared<Tunkio::FileFiller>(bufferSize, optional, verify);

				if (fileFiller->HasContent())
				{
					instance->AddFiller(fileFiller);
					return true;
				}
			}
			return false;

		case TunkioFillType::RandomFill:
			instance->AddFiller(new Tunkio::RandomFiller(bufferSize, verify));
			return true;

	}

	return false;
}

void TUNKIO_CALLING_CONVENTION TunkioSetWipeStartedCallback(
	TunkioHandle* handle,
	TunkioWipeStartedCallback* callback)
{
	Set(handle, &Tunkio::IWorkload::SetWipeStartedCallback, callback);
}

void TUNKIO_CALLING_CONVENTION TunkioSetPassStartedCallback(
	TunkioHandle* handle,
	TunkioPassStartedCallback* callback)
{
	Set(handle, &Tunkio::IWorkload::SetPassStartedCallback, callback);
}

void TUNKIO_CALLING_CONVENTION TunkioSetProgressCallback(
	TunkioHandle* handle,
	TunkioProgressCallback* callback)
{
	Set(handle, &Tunkio::IWorkload::SetProgressCallback, callback);
}

void TUNKIO_CALLING_CONVENTION TunkioSetPassCompletedCallback(
	TunkioHandle* handle,
	TunkioPassCompletedCallback* callback)
{
	Set(handle, &Tunkio::IWorkload::SetPassCompletedCallback, callback);
}

void TUNKIO_CALLING_CONVENTION TunkioSetWipeCompletedCallback(
	TunkioHandle* handle,
	TunkioWipeCompletedCallback* callback)
{
	Set(handle, &Tunkio::IWorkload::SetWipeCompletedCallback, callback);
}

void TUNKIO_CALLING_CONVENTION TunkioSetErrorCallback(
	TunkioHandle* handle,
	TunkioErrorCallback* callback)
{
	Set(handle, &Tunkio::IWorkload::SetErrorCallback, callback);
}

bool TUNKIO_CALLING_CONVENTION TunkioRun(TunkioHandle* handle)
{
	const auto operation = reinterpret_cast<Tunkio::IWorkload*>(handle);

	if (!operation)
	{
		return false;
	}

	return operation->Run();
}

void TUNKIO_CALLING_CONVENTION TunkioFree(TunkioHandle* handle)
{
	if (handle)
	{
		delete reinterpret_cast<Tunkio::IWorkload*>(handle);
	}
}
