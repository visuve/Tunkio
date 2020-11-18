#include "TunkioAPI-PCH.hpp"
#include "TunkioAPI.h"
#include "TunkioErrorCodes.hpp"
#include "TunkioDataUnits.hpp"

#include "Workloads/TunkioWorkload.hpp"
#include "Workloads/TunkioFileWipe.hpp"
#include "Workloads/TunkioDirectoryWipe.hpp"
#include "Workloads/TunkioDriveWipe.hpp"

#include "FillProviders/TunkioFillProvider.hpp"
#include "FillProviders/TunkioCharFiller.hpp"
#include "FillProviders/TunkioStringFiller.hpp"
#include "FillProviders/TunkioRandomFiller.hpp"

template <typename T>
bool Assign(TunkioHandle* handle, T(Tunkio::IWorkload::* field), T value)
{
	const auto instance = reinterpret_cast<Tunkio::IWorkload*>(handle);

	if (!instance)
	{
		return false;
	}

	instance->*field = value;

	return true;
}

template<typename Base>
inline bool IsInstanceOf(const TunkioHandle* handle)
{
	auto instance = reinterpret_cast<const Tunkio::IWorkload*>(handle);
	return dynamic_cast<const Base*>(instance) != nullptr;
}

TunkioHandle* TUNKIO_CALLING_CONVENTION TunkioInitialize(void* context, const char* path, TunkioTargetType type)
{
	if (!path)
	{
		return nullptr;
	}

	switch (type)
	{
		case TunkioTargetType::File:
			return reinterpret_cast<TunkioHandle*>(new Tunkio::FileWipe(context, path));
		case TunkioTargetType::Directory:
			return reinterpret_cast<TunkioHandle*>(new Tunkio::DirectoryWipe(context, path));
		case TunkioTargetType::Drive:
			return reinterpret_cast<TunkioHandle*>(new Tunkio::DriveWipe(context, path));
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

	constexpr Tunkio::DataUnit::Mebibytes mebibyte(1);

	switch (round)
	{
		case TunkioFillType::Zeroes:
			instance->m_fillers.emplace(new Tunkio::CharFiller(mebibyte, 0x00, verify));
			return true;

		case TunkioFillType::Ones:
			instance->m_fillers.emplace(new Tunkio::CharFiller(mebibyte, 0xFF, verify));
			return true;

		case TunkioFillType::Character:
			if (!optional || strlen(optional) != 1)
			{
				return false;
			}

			instance->m_fillers.emplace(new Tunkio::CharFiller(mebibyte, optional[0], verify));
			return true;

		case TunkioFillType::Sentence:
			if (!optional || !strlen(optional))
			{
				return false;
			}

			instance->m_fillers.emplace(new Tunkio::StringFiller(mebibyte, optional, verify));
			return true;

		case TunkioFillType::Random:
			instance->m_fillers.emplace(new Tunkio::RandomFiller(mebibyte, verify));
			return true;

	}

	return false;
}

bool TUNKIO_CALLING_CONVENTION TunkioSetStartedCallback(
	TunkioHandle* handle,
	TunkioStartedCallback* callback)
{
	return Assign(handle, &Tunkio::IWorkload::m_startedCallback, callback);
}

bool TUNKIO_CALLING_CONVENTION TunkioSetIterationStartedCallback(
	TunkioHandle* handle,
	TunkioIterationStartedCallback* callback)
{
	return Assign(handle, &Tunkio::IWorkload::m_iterationStartedCallback, callback);
}

bool TUNKIO_CALLING_CONVENTION TunkioSetProgressCallback(
	TunkioHandle* handle,
	TunkioProgressCallback* callback)
{
	return Assign(handle, &Tunkio::IWorkload::m_progressCallback, callback);
}

bool TUNKIO_CALLING_CONVENTION TunkioSetIterationCompletedCallback(
	TunkioHandle* handle,
	TunkioIterationCompletedCallback* callback)
{
	return Assign(handle, &Tunkio::IWorkload::m_iterationCompletedCallback, callback);;
}

bool TUNKIO_CALLING_CONVENTION TunkioSetCompletedCallback(
	TunkioHandle* handle,
	TunkioCompletedCallback* callback)
{
	return Assign(handle, &Tunkio::IWorkload::m_completedCallback, callback);
}

bool TUNKIO_CALLING_CONVENTION TunkioSetErrorCallback(
	TunkioHandle* handle,
	TunkioErrorCallback* callback)
{
	return Assign(handle, &Tunkio::IWorkload::m_errorCallback, callback);
}

bool TUNKIO_CALLING_CONVENTION TunkioSetRemoveAfterFill(TunkioHandle* handle, bool remove)
{
	if (IsInstanceOf<Tunkio::DriveWipe>(handle) && remove)
	{
		return false; // Devices cannot be deleted
	}

	return Assign(handle, &Tunkio::IWorkload::m_removeAfterFill, remove);
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
