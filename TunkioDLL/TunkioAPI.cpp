#include "PCH.hpp"
#include "TunkioArgs.hpp"
#include "TunkioIO.hpp"
#include "TunkioUnits.hpp"
#include "TunkioTiming.hpp"
#include "TunkioEncoding.hpp"
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

    uint32_t WipeFile(const IO::Path& path, bool remove)
    {
        {
            if (!IO::FileSystem::exists(path))
            {
                std::wcerr << L"File: " << path << L" not found" << std::endl;
                return ERROR_FILE_NOT_FOUND;
            }

            std::wcout << L"Wiping file: " << path << std::endl;

            IO::FileStream file = IO::File::Open(path);

            if (!file)
            {
                const uint32_t error = GetLastError();
                std::wcerr << L"Could not open file: " << error << L" / " << Help::Win32ErrorToString(error) << std::endl;
                return error;
            }

            uint64_t bytesLeft = IO::File::Size(path);

            if (!bytesLeft)
            {
                const uint32_t error = GetLastError();
                std::wcerr << L"Failed to file size: " << error << L" / " << Help::Win32ErrorToString(error) << std::endl;
                return error;
            }

            uint64_t writtenBytesTotal = 0;
            uint32_t error = ERROR_SUCCESS;
            const Timing::Timer stopWatch;

            const auto progress = [](uint64_t bytesWritten, uint64_t secondsElapsed) -> void
            {
                const uint64_t megabytesWritten = bytesWritten / Units::MegaByte;
                std::wcout << megabytesWritten << L" megabytes written. Speed " << megabytesWritten / secondsElapsed << " MB/s" << std::endl;
            };

            if (!IO::File::Fill(file, bytesLeft, writtenBytesTotal, progress))
            {
                error = GetLastError();
                std::wcerr << L"Write operation failed: " << error << L" / " << Help::Win32ErrorToString(error) << std::endl;
            }

            std::wcout << L"Wiped: " << writtenBytesTotal << L" bytes. " << bytesLeft << L" Left unwiped" << std::endl;
            std::wcout << L"Took: " << stopWatch.Elapsed() << std::endl;
            
            if (error != ERROR_SUCCESS)
            {
                return error;
            }
        }

        if (remove && !IO::File::Remove(path))
        {
            const uint32_t error = GetLastError();
            std::wcerr << L"Failed to remove file: " << error << L" / " << Help::Win32ErrorToString(error) << std::endl;
            return error;
        }

        return ERROR_SUCCESS;
    }

    uint32_t WipeDirectory(const std::wstring&, bool)
    {
        std::wcerr << L"Wiping directories not yet supported";
        return ERROR_NOT_SUPPORTED;
    }

    uint32_t WipeVolume(const std::wstring& path)
    {
        std::wcout << L"Wiping volume: " << path << std::endl;

        const IO::AutoHandle volume(IO::Volume::Open(path));

        if (!volume.IsValid())
        {
            const uint32_t error = GetLastError();
            std::wcerr << L"Could not open volume: " << error << L" / " << Help::Win32ErrorToString(error) << std::endl;
            return error;
        }

        uint64_t bytesLeft = IO::Volume::Size(volume);

        if (!bytesLeft)
        {
            const uint32_t error = GetLastError();
            std::wcerr << L"Failed to get disk geometry: " << error << L" / " << Help::Win32ErrorToString(error) << std::endl;
            return error;
        }

        std::wcout << L"Bytes to wipe: " << bytesLeft << std::endl;

        uint64_t writtenBytesTotal = 0;
        uint32_t error = ERROR_SUCCESS;
        const Timing::Timer stopWatch;

        const auto progress = [](uint64_t bytesWritten, uint64_t secondsElapsed) -> void
        {
            const uint64_t megabytesWritten = bytesWritten / Units::MegaByte;
            std::wcout << megabytesWritten << L" megabytes written. Speed " << megabytesWritten / secondsElapsed << " MB/s" << std::endl;
        };

        if (!IO::Volume::Fill(volume, bytesLeft, writtenBytesTotal, progress))
        {
            error = GetLastError();
            std::wcerr << L"Write operation failed: " << error << L" / " << Help::Win32ErrorToString(error) << std::endl;
        }

        std::wcout << L"Wiped: " << writtenBytesTotal << L" bytes. " << bytesLeft << L" Left unwiped" << std::endl;
        std::wcout << L"Took: " << stopWatch.Elapsed() << std::endl;
        return error;
    }

    uint32_t Exec(const std::vector<std::wstring>& args)
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
            std::wcerr << L"Target auto detecion not yet supported";
            return ERROR_NOT_SUPPORTED;

        case Tunkio::Args::Target::File:
            return WipeFile(path, remove);

        case Tunkio::Args::Target::Directory:
            return WipeDirectory(path, remove);

        case Tunkio::Args::Target::Volume:
            return WipeVolume(path);
        }

        return ERROR_BAD_ARGUMENTS;
    }
}

unsigned long __stdcall TunkioExecuteW(int argc, wchar_t* argv[])
{
    return Tunkio::Exec({ argv + 1, argv + argc });
}

unsigned long __stdcall TunkioExecuteA(int argc, char* argv[])
{
    using namespace Tunkio;
    return Exec(Encoding::AnsiToUnicode(std::vector<std::string>(argv + 1, argv + argc)));
}