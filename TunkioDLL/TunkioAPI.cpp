#include "PCH.hpp"
#include "TunkioAPI.h"
#include "TunkioErrorCodes.hpp"

#include "Workloads/TunkioWorkload.hpp"
#include "Workloads/TunkioFileWipe.hpp"
#include "Workloads/TunkioDirectoryWipe.hpp"
#include "Workloads/TunkioDriveWipe.hpp"

TunkioHandle* TUNKIO_CALLING_CONVENTION TunkioInitialize(const char* path, TunkioTargetType type)
{
	if (!path)
	{
		return nullptr;
	}

	switch (type)
	{
		case TunkioTargetType::File:
			return reinterpret_cast<TunkioHandle*>(new Tunkio::FileWipe(path));
		case TunkioTargetType::Directory:
			return reinterpret_cast<TunkioHandle*>(new Tunkio::DirectoryWipe(path));
		case TunkioTargetType::Drive:
			return reinterpret_cast<TunkioHandle*>(new Tunkio::DriveWipe(path));
	}

	return nullptr;
}

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

bool TUNKIO_CALLING_CONVENTION TunkioSetFillMode(TunkioHandle* handle, TunkioFillMode mode)
{
	return Assign(handle, &Tunkio::IWorkload::m_fillMode, mode);
}

bool TUNKIO_CALLING_CONVENTION TunkioSetStartedCallback(TunkioHandle* handle, TunkioStartedCallback* callback)
{
	return Assign(handle, &Tunkio::IWorkload::m_startedCallback, callback);
}

bool TUNKIO_CALLING_CONVENTION TunkioSetProgressCallback(TunkioHandle* handle, TunkioProgressCallback* callback)
{
	return Assign(handle, &Tunkio::IWorkload::m_progressCallback, callback);
}

bool TUNKIO_CALLING_CONVENTION TunkioSetErrorCallback(TunkioHandle* handle, TunkioErrorCallback* callback)
{
	return Assign(handle, &Tunkio::IWorkload::m_errorCallback, callback);
}

bool TUNKIO_CALLING_CONVENTION TunkioSetCompletedCallback(TunkioHandle* handle, TunkioCompletedCallback* callback)
{
	return Assign(handle, &Tunkio::IWorkload::m_completedCallback, callback);
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