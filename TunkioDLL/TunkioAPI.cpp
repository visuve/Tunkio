#include "PCH.hpp"
#include "TunkioAPI.h"
#include "TunkioArgs.hpp"
#include "TunkioEncoding.hpp"
#include "TunkioTiming.hpp"

#include "TunkioOutput.hpp"
#include "TunkioFileSystem.hpp"
#include "TunkioNative.hpp"

namespace Tunkio
{
    uint32_t WipeFile(const Path& path, bool remove, TunkioProgressCallback progress)
    {
        {
            if (!FileSystem::exists(path))
            {
                std::wcerr << L"File: " << path << L" not found" << std::endl;
                return ERROR_FILE_NOT_FOUND;
            }

            FileStream file(path, std::ios::in | std::ios::out | std::ios::binary);

            if (!file)
            {
                const uint32_t error = GetLastError();
                std::wcerr << L"Could not open file: " << error << L" / " << Native::Win32::ErrorToString(error) << std::endl;
                return error;
            }

            uint64_t bytesLeft = FileSystem::file_size(path);

            if (!bytesLeft)
            {
                const uint32_t error = GetLastError();
                std::wcerr << L"Failed to file size: " << error << L" / " << Native::Win32::ErrorToString(error) << std::endl;
                return error;
            }

            uint64_t writtenBytesTotal = 0;
            uint32_t error = ERROR_SUCCESS;
            const Timing::Timer stopWatch;

            if (!Output::Fill(file, bytesLeft, writtenBytesTotal, progress))
            {
                error = GetLastError();
                std::wcerr << L"Write operation failed: " << error << L" / " << Native::Win32::ErrorToString(error) << std::endl;
            }

            std::wcout << L"Wiped: " << writtenBytesTotal << L" bytes. " << bytesLeft << L" Left unwiped" << std::endl;
            std::wcout << L"Took: " << stopWatch.Elapsed() << std::endl;

            if (error != ERROR_SUCCESS)
            {
                return error;
            }
        }

        if (remove && !FileSystem::remove(path))
        {
            const uint32_t error = GetLastError();
            std::wcerr << L"Failed to remove file: " << error << L" / " << Native::Win32::ErrorToString(error) << std::endl;
            return error;
        }

        return ERROR_SUCCESS;
    }

    uint32_t WipeDirectory(const Path&, bool, TunkioProgressCallback)
    {
        std::wcerr << L"Wiping directories not yet supported";
        return ERROR_NOT_SUPPORTED;
    }

    uint32_t WipeVolume(const Path& path, TunkioProgressCallback progress)
    {
        const Native::Win32::AutoHandle volume(Native::Win32::Open(path));

        if (!volume.IsValid())
        {
            const uint32_t error = GetLastError();
            std::wcerr << L"Could not open volume: " << error << L" / " << Native::Win32::ErrorToString(error) << std::endl;
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
            std::wcerr << L"Failed to get disk geometry: " << error << L" / " << Native::Win32::ErrorToString(error) << std::endl;
            return error;
        }

        std::wcout << L"Bytes to wipe: " << bytesLeft << std::endl;

        uint64_t writtenBytesTotal = 0;
        uint32_t error = ERROR_SUCCESS;
        const Timing::Timer stopWatch;

        if (!Native::Win32::Fill(volume, bytesLeft, writtenBytesTotal, progress))
        {
            error = GetLastError();
            std::wcerr << L"Write operation failed: " << error << L" / " << Native::Win32::ErrorToString(error) << std::endl;
        }

        std::wcout << L"Wiped: " << writtenBytesTotal << L" bytes. " << bytesLeft << L" Left unwiped" << std::endl;
        std::wcout << L"Took: " << stopWatch.Elapsed() << std::endl;
        return error;
    }

    template <typename C>
    uint32_t Exec(const std::array<Args::Argument<C>, 4>& args, TunkioProgressCallback progress)
    {
        // TODO: this ain't the prettiest
        const auto path = args[0].Value<std::basic_string<C>>();

        const bool remove = args[3].Value<bool>();

        switch (args[1].Value<Args::Target>())
        {
        case Tunkio::Args::Target::AutoDetect:
            std::wcerr << L"Target auto detecion not yet supported" << std::endl;
            return ERROR_NOT_SUPPORTED;

        case Tunkio::Args::Target::File:
            return WipeFile(path, remove, progress);

        case Tunkio::Args::Target::Directory:
            return WipeDirectory(path, remove, progress);

        case Tunkio::Args::Target::MassMedia:
            return WipeVolume(path, progress);
        }

        return ERROR_BAD_ARGUMENTS;
    }
}

unsigned long __stdcall TunkioExecuteW(int argc, wchar_t* argv[], TunkioProgressCallback progress)
{
    using namespace Tunkio;

    try
    {
        std::array<Args::WideArgument, 4> args =
        {
            Args::WideArgument(true, L"--path=", std::wstring()),
            Args::WideArgument(false, L"--target=", Args::Target::AutoDetect),
            Args::WideArgument(false, L"--mode=", Args::Mode::Zeroes),
            Args::WideArgument(false, L"--remove=", false),
        };

        if (!Args::Parse(args, std::vector<std::wstring>({ argv + 1, argv + argc })))
        {
            return ERROR_BAD_ARGUMENTS;
        }

        return Tunkio::Exec(args, progress);
    }
    catch (const std::exception& e)
    {
        std::wcout << L"An exception occurred: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::wcout << L"An unknown exception occurred." << std::endl;
    }

    return ERROR_UNHANDLED_EXCEPTION;
}

unsigned long __stdcall TunkioExecuteA(int argc, char* argv[], TunkioProgressCallback progress)
{
    using namespace Tunkio;

    try
    {
        std::array<Args::NarrowArgument, 4> args =
        {
            Args::NarrowArgument(true, "--path=", std::string()),
            Args::NarrowArgument(false, "--target=", Args::Target::AutoDetect),
            Args::NarrowArgument(false, "--mode=", Args::Mode::Zeroes),
            Args::NarrowArgument(false, "--remove=", false),
        };

        if (!Args::Parse(args, std::vector<std::string>({ argv + 1, argv + argc })))
        {
            return ERROR_BAD_ARGUMENTS;
        }

        return Tunkio::Exec(args, progress);
    }
    catch (std::exception& e)
    {
        std::wcout << L"An exception occurred: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::wcout << L"An unknown exception occurred." << std::endl;
    }

    return ERROR_UNHANDLED_EXCEPTION;
}