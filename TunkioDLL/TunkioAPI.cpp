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

    uint32_t WipeDirectory(const std::wstring&, bool, TunkioProgressCallback)
    {
        std::wcerr << L"Wiping directories not yet supported";
        return ERROR_NOT_SUPPORTED;
    }

    uint32_t WipeVolume(const std::wstring& path, TunkioProgressCallback progress)
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

    uint32_t Exec(const std::vector<std::wstring>& args, TunkioProgressCallback progress)
    {
        if (!Args::Parse(args))
        {
            return ERROR_BAD_ARGUMENTS;
        }

        // TODO: this ain't the prettiest
        const std::wstring path = Args::Arguments[0].Value<std::wstring>();
        const bool remove = Args::Arguments[3].Value<bool>();

        switch (Args::Arguments[1].Value<Args::Target>())
        {
        case Tunkio::Args::Target::AutoDetect:
            std::wcerr << L"Target auto detecion not yet supported" << std::endl;
            return ERROR_NOT_SUPPORTED;

        case Tunkio::Args::Target::File:
            return WipeFile(path, remove, progress);

        case Tunkio::Args::Target::Directory:
            return WipeDirectory(path, remove, progress);

        case Tunkio::Args::Target::Volume:
            return WipeVolume(path, progress);
        }

        return ERROR_BAD_ARGUMENTS;
    }
}

unsigned long __stdcall TunkioExecuteW(int argc, wchar_t* argv[], TunkioProgressCallback progress)
{
    return Tunkio::Exec({ argv + 1, argv + argc }, progress);
}

unsigned long __stdcall TunkioExecuteA(int argc, char* argv[], TunkioProgressCallback progress)
{
    using namespace Tunkio;
    return Exec(Encoding::AnsiToUnicode(std::vector<std::string>(argv + 1, argv + argc)), progress);
}