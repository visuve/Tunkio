#include "PCH.hpp"
#include "TunkioAPI.h"
#include "TunkioEncoding.hpp"
#include "TunkioErrorCodes.hpp"

#include "Strategies/ITunkioOperation.hpp"
#include "Strategies/TunkioFileWipe.hpp"
#include "Strategies/TunkioDeviceWipe.hpp"

//namespace Tunkio
//{
//    FileWipeStrategy* WipeFile(const std::string& path, TunkioCallbacks callbacks, bool removeOnExit)
//    {
//        auto strategy = new FileWipeStrategy(path, callbacks, removeOnExit);
//
//        if (!strategy->Exists())
//        {
//            callbacks.ErrorCallback(ErrorCode::FileNotFound, 0);
//            return strategy;
//        }
//
//        if (!strategy->Size())
//        {
//            callbacks.ErrorCallback(ErrorCode::FileEmpty, 0);
//        }
//        else
//        {
//            strategy->Fill();
//        }
//
//        return strategy;
//    }
//
//    TunkioHandle* WipeDirectory(const std::string& /*path*/, TunkioCallbacks /*callbacks*/, bool/* removeOnExit*/)
//    {
//        std::cerr << "Wiping directories not yet implemented";
//        return nullptr;
//    }
//
//    TunkioHandle* WipeDevice(const std::string& /*path*/, TunkioCallbacks /*callbacks*/, bool/* removeOnExit*/)
//    {
//        std::cerr << "Wiping deviced not yet implemented";
//        return nullptr;
//    }
//}

TunkioHandle* __cdecl TunkioCreate(const TunkioOptions* options)
{
    if (!options)
    {
        return nullptr;
    }

    // std::string path(options->Path.Data, options->Path.Length);

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
    auto operation = reinterpret_cast<Tunkio::IOperation*>(handle);

    if (!operation)
    {
        return Tunkio::ErrorCode::InvalidArgument;
    }

    return operation->Run();
}

void __cdecl TunkioFree(TunkioHandle* handle)
{
    delete reinterpret_cast<Tunkio::IOperation*>(handle);
}