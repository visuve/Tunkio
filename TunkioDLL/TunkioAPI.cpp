#include "PCH.hpp"
#include "TunkioAPI.h"
#include "TunkioEncoding.hpp"
#include "TunkioErrorCodes.hpp"

#include "Strategies/ITunkioOperation.hpp"
#include "Strategies/TunkioFileWipe.hpp"
#include "Strategies/TunkioDeviceWipe.hpp"

namespace Tunkio
{
    uint32_t Run(Tunkio::IOperation* operation)
    {
        // Pre-checks
        if (!operation->Exists())
        {
            return ErrorCode::FileNotFound;
        }

        if (!operation->Size())
        {
            return ErrorCode::FileEmpty;
        }

        return operation->Run();
    }
}

TunkioHandle* __cdecl TunkioCreate(const TunkioOptions* options)
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

uint32_t __cdecl TunkioRun(TunkioHandle* handle)
{
    const auto operation = reinterpret_cast<Tunkio::IOperation*>(handle);

    if (!operation)
    {
        return Tunkio::ErrorCode::InvalidArgument;
    }

    return Tunkio::Run(operation);
}

void __cdecl TunkioFree(TunkioHandle* handle)
{
    if (handle)
    {
        delete reinterpret_cast<Tunkio::IOperation*>(handle);
    }
}