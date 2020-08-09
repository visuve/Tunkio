#include "PCH.hpp"
#include "TunkioAPI.h"
#include "TunkioErrorCodes.hpp"

#include "ITunkioOperation.hpp"
#include "TunkioFileWipe.hpp"
#include "TunkioDeviceWipe.hpp"

TunkioHandle* CALLING_CONVENTION TunkioCreate(const TunkioOptions* options)
{
    if (!options)
    {
        return nullptr;
    }

    switch (options->Target)
    {
        case TunkioTarget::File:
            return reinterpret_cast<TunkioHandle*>(new Tunkio::FileWipe(options));
        case TunkioTarget::Device:
            return reinterpret_cast<TunkioHandle*>(new Tunkio::DeviceWipe(options));
        default:
            std::cerr << "Not implemented yet!" << std::endl;
    }

    return nullptr;
}

bool CALLING_CONVENTION TunkioRun(TunkioHandle* handle)
{
    const auto operation = reinterpret_cast<Tunkio::IOperation*>(handle);

    if (!operation)
    {
        return false;
    }

    return operation->Run();
}

void CALLING_CONVENTION TunkioFree(TunkioHandle* handle)
{
    if (handle)
    {
        delete reinterpret_cast<Tunkio::IOperation*>(handle);
    }
}