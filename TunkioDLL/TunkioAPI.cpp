#include "PCH.hpp"
#include "TunkioArgs.hpp"
#include "TunkioIO.hpp"
#include "TunkioTimer.hpp"
#include "TunkioAPI.h"

namespace Tunkio
{
    namespace Help
    {
        std::wstring Win32ErrorToString(const uint32_t error)
        {
            constexpr uint32_t flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
            constexpr uint32_t langId = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
            wchar_t buffer[Units::KiloByte] = { 0 };
            const uint32_t size = FormatMessage(flags, nullptr, error, langId, buffer, Units::KiloByte, nullptr);
            return std::wstring(buffer, size);
        }
    }

    uint32_t WipeData(const Tunkio::IO::AutoHandle &handle, uint64_t bytesLeft)
    {
        std::wcout << L"Bytes to wipe: " << bytesLeft << std::endl;

        uint64_t writtenBytesTotal = 0;
        uint32_t error = ERROR_SUCCESS;
        const Timer<Units::Seconds> stopWatch;

        const auto progress = [](uint64_t bytesWritten, uint64_t secondsElapsed) -> void
        {
            const uint64_t megabytesWritten = bytesWritten / Units::MegaByte;
            std::wcout << megabytesWritten << L" megabytes written. Speed " << megabytesWritten / secondsElapsed << " MB/s" << std::endl;
        };

        if (!IO::Wipe(handle, bytesLeft, writtenBytesTotal, progress))
        {
            error = GetLastError();
            std::wcerr << L"Write operation failed: " << error << L" / " << Help::Win32ErrorToString(error) << std::endl;
        }

        std::wcout << L"Wiped: " << writtenBytesTotal << L" bytes. " << bytesLeft << L" Left unwiped" << std::endl;
        std::wcout << L"Took: " << stopWatch << std::endl;
        return error;
    }

    uint32_t WipeFile(const std::wstring& path)
    {
        std::wcout << L"Wiping file: " << path << std::endl;

        const IO::AutoHandle file(IO::Open(path));

        if (!file.IsValid())
        {
            const uint32_t error = GetLastError();
            std::wcerr << L"Could not open file: " << error << L" / " << Help::Win32ErrorToString(error) << std::endl;
            return error;
        }

        uint64_t bytesLeft = IO::FileSize(file);

        if (!bytesLeft)
        {
            const uint32_t error = GetLastError();
            std::wcerr << L"Failed to file size: " << error << L" / " << Help::Win32ErrorToString(error) << std::endl;
            return error;
        }

        return WipeData(file, bytesLeft);
    }

    uint32_t WipeVolume(const std::wstring& path)
    {
        std::wcout << L"Wiping volume: " << path << std::endl;

        const IO::AutoHandle volume(IO::Open(path));

        if (!volume.IsValid())
        {
            const uint32_t error = GetLastError();
            std::wcerr << L"Could not open volume: " << error << L" / " << Help::Win32ErrorToString(error) << std::endl;
            return error;
        }

        uint64_t bytesLeft = IO::VolumeSize(volume);

        if (!bytesLeft)
        {
            const uint32_t error = GetLastError();
            std::wcerr << L"Failed to get disk geometry: " << error << L" / " << Help::Win32ErrorToString(error) << std::endl;
            return error;
        }

        return WipeData(volume, bytesLeft);
    }

    uint32_t Exec(const std::wstring& path, Args::Target target)
    {
        switch (target)
        {
        case Tunkio::Args::Target::AutoDetect:
            std::wcerr << L"Target auto detecion not yet supported";
            return ERROR_NOT_SUPPORTED;

        case Tunkio::Args::Target::File:
            return WipeFile(path);

        case Tunkio::Args::Target::Directory:
            std::wcerr << L"Wiping directories not yet supported";
            return ERROR_NOT_SUPPORTED;

        case Tunkio::Args::Target::Volume:
            return WipeVolume(path);
        }

        return ERROR_BAD_ARGUMENTS;
    }
}

unsigned long __stdcall TunkioExecuteW(int argc, wchar_t* argv[])
{
    using namespace Tunkio;

    if (!Args::Parse({ argv + 1, argv + argc }))
    {
        return ERROR_BAD_ARGUMENTS;
    }

    // TODO: this ain't the prettiest...
    return Exec(
        Args::Arguments[0].Value<std::wstring>(), 
        Args::Arguments[1].Value<Args::Target>());
}

unsigned long __stdcall TunkioExecuteA(int /*argc*/, char* argv[])
{
    // TODO: convert args to wstring 

    std::cout << argv[0];

    return ERROR_NOT_SUPPORTED;
}
