#include "PCH.hpp"
#include "TunkioAPI.h"
#include "TunkioEncoding.hpp"
#include "TunkioTiming.hpp"

#include "TunkioOutput.hpp"
#include "TunkioFileSystem.hpp"
#include "TunkioNative.hpp"
#include "TunkioExitCodes.hpp"

namespace Tunkio
{
    uint32_t WipeFile(const Path& path, bool remove, TunkioProgressCallback progress)
    {
        {
            if (!FileSystem::exists(path))
            {
                std::cerr << "File: " << path << " not found" << std::endl;
                return Tunkio::ExitCode::FileNotFound;
            }

            FileStream file(path, std::ios::in | std::ios::out | std::ios::binary);

            if (!file)
            {
                const uint32_t error = GetLastError();
                std::cerr << "Could not open file: " << error << " / " << Native::Win32::ErrorToString(error) << std::endl;
                return error;
            }

            uint64_t bytesLeft = FileSystem::file_size(path);

            if (!bytesLeft)
            {
                const uint32_t error = GetLastError();
                std::cerr << "Failed to file size: " << error << " / " << Native::Win32::ErrorToString(error) << std::endl;
                return error;
            }

            uint64_t writtenBytesTotal = 0;
            uint32_t error = ExitCode::Success;
            const Timing::Timer stopWatch;

            if (!Output::Fill(file, bytesLeft, writtenBytesTotal, progress))
            {
                error = GetLastError();
                std::cerr << "Write operation failed: " << error << " / " << Native::Win32::ErrorToString(error) << std::endl;
            }

            std::cout << "Wiped: " << writtenBytesTotal << " bytes. " << bytesLeft << " Left unwiped" << std::endl;
            std::cout << "Took: " << stopWatch.Elapsed() << std::endl;

            if (error != ExitCode::Success)
            {
                return error;
            }
        }

        if (remove && !FileSystem::remove(path))
        {
            const uint32_t error = GetLastError();
            std::cerr << "Failed to remove file: " << error << " / " << Native::Win32::ErrorToString(error) << std::endl;
            return error;
        }

        return ExitCode::Success;
    }

    uint32_t WipeDirectory(const Path&, bool, TunkioProgressCallback)
    {
        std::cerr << "Wiping directories not yet implemented";
        return Tunkio::ExitCode::NotImplemented;
    }

    uint32_t WipeVolume(const Path& path, TunkioProgressCallback progress)
    {
        const Native::Win32::AutoHandle volume(Native::Win32::Open(path));

        if (!volume.IsValid())
        {
            const uint32_t error = GetLastError();
            std::cerr << "Could not open volume: " << error << " / " << Native::Win32::ErrorToString(error) << std::endl;
            return error;
        }

        // NOTE: it's simply impossible to get the volume size otherwise.
        // std::filesystem::space() fails
        // std::file_system::file_size() fails
        // std::fstream.tellp() returns too little
        uint64_t bytesLeft = Native::Win32::VolumeSize(volume);

        if (!bytesLeft)
        {
            const uint32_t error = GetLastError();
            std::cerr << "Failed to get disk geometry: " << error << " / " << Native::Win32::ErrorToString(error) << std::endl;
            return error;
        }

        std::cout << "Bytes to wipe: " << bytesLeft << std::endl;

        uint64_t writtenBytesTotal = 0;
        uint32_t error = ExitCode::Success;
        const Timing::Timer stopWatch;

        if (!Native::Win32::Fill(volume, bytesLeft, writtenBytesTotal, progress))
        {
            error = GetLastError();
            std::cerr << "Write operation failed: " << error << " / " << Native::Win32::ErrorToString(error) << std::endl;
        }

        std::cout << "Wiped: " << writtenBytesTotal << " bytes. " << bytesLeft << " Left unwiped" << std::endl;
        std::cout << "Took: " << stopWatch.Elapsed() << std::endl;
        return error;
    }
}

uint32_t __cdecl TunkioExecute(const TunkioOptions* options)
{
    if (!options)
    {
        return Tunkio::ExitCode::InvalidArgument;
    }

    const Path path(std::string(options->Path.Data, options->Path.Length));

    switch (options->Target)
    {
        case TunkioTarget::AutoDetect:
            std::cerr << "Target auto detecion not yet supported" << std::endl;
            return Tunkio::ExitCode::NotImplemented;

        case TunkioTarget::File:
            return Tunkio::WipeFile(path, options->Remove, options->ProgressCallback);

        case TunkioTarget::Directory:
            return Tunkio::WipeDirectory(path, options->Remove, options->ProgressCallback);

        case TunkioTarget::Volume:
            return Tunkio::WipeVolume(path, options->ProgressCallback);
    }

    return 0;
}