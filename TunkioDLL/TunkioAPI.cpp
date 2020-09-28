#include "PCH.hpp"
#include "TunkioAPI.h"
#include "TunkioErrorCodes.hpp"

#include "TunkioOperation.hpp"
#include "TunkioFileWipe.hpp"
#include "TunkioDirectoryWipe.hpp"
#include "TunkioDeviceWipe.hpp"

TunkioHandle* TUNKIO_CALLING_CONVENTION TunkioCreate(const TunkioOptions* options)
{
	if (!options)
	{
		return nullptr;
	}

	switch (options->Target)
	{
		case TunkioTargetType::File:
			return reinterpret_cast<TunkioHandle*>(new Tunkio::FileWipe(options));
		case TunkioTargetType::Directory:
			return reinterpret_cast<TunkioHandle*>(new Tunkio::DirectoryWipe(options));
		case TunkioTargetType::Device:
			return reinterpret_cast<TunkioHandle*>(new Tunkio::DeviceWipe(options));
	}

	return nullptr;
}

bool TUNKIO_CALLING_CONVENTION TunkioRun(TunkioHandle* handle)
{
	const auto operation = reinterpret_cast<Tunkio::IOperation*>(handle);

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
		delete reinterpret_cast<Tunkio::IOperation*>(handle);
	}
}