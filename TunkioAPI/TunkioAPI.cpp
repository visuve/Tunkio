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
		case TunkioTargetType::FileWipe:
			return reinterpret_cast<TunkioHandle*>(new Tunkio::FileWipe(context, path));
		case TunkioTargetType::DirectoryWipe:
			return reinterpret_cast<TunkioHandle*>(new Tunkio::DirectoryWipe(context, path));
		case TunkioTargetType::DriveWipe:
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

	constexpr Tunkio::DataUnit::Mebibytes bufferSize(1);

	switch (round)
	{
		case TunkioFillType::ZeroFill:
			instance->m_fillers.emplace(new Tunkio::CharFiller(bufferSize, 0x00, verify));
			return true;

		case TunkioFillType::OneFill:
			instance->m_fillers.emplace(new Tunkio::CharFiller(bufferSize, 0xFF, verify));
			return true;

		case TunkioFillType::CharacterFill:
			if (!optional || strlen(optional) != 1)
			{
				return false;
			}

			instance->m_fillers.emplace(new Tunkio::CharFiller(bufferSize, optional[0], verify));
			return true;

		case TunkioFillType::SentenceFill:
			if (!optional || !strlen(optional))
			{
				return false;
			}

			instance->m_fillers.emplace(new Tunkio::SentenceFiller(bufferSize, optional, verify));
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
					instance->m_fillers.emplace(fileFiller);
					return true;
				}
			}
			return false;

		case TunkioFillType::RandomFill:
			instance->m_fillers.emplace(new Tunkio::RandomFiller(bufferSize, verify));
			return true;

	}

	return false;
}

bool TUNKIO_CALLING_CONVENTION TunkioSetWipeStartedCallback(
	TunkioHandle* handle,
	TunkioWipeStartedCallback* callback)
{
	return Assign(handle, &Tunkio::IWorkload::m_startedCallback, callback);
}

bool TUNKIO_CALLING_CONVENTION TunkioSetPassStartedCallback(
	TunkioHandle* handle,
	TunkioPassStartedCallback* callback)
{
	return Assign(handle, &Tunkio::IWorkload::m_passStartedCallback, callback);
}

bool TUNKIO_CALLING_CONVENTION TunkioSetProgressCallback(
	TunkioHandle* handle,
	TunkioProgressCallback* callback)
{
	return Assign(handle, &Tunkio::IWorkload::m_progressCallback, callback);
}

bool TUNKIO_CALLING_CONVENTION TunkioSetPassCompletedCallback(
	TunkioHandle* handle,
	TunkioPassCompletedCallback* callback)
{
	return Assign(handle, &Tunkio::IWorkload::m_passCompletedCallback, callback);;
}

bool TUNKIO_CALLING_CONVENTION TunkioSetWipeCompletedCallback(
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
