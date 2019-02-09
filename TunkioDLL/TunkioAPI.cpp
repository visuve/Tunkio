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

    uint32_t Exec(const std::wstring& path)
    {
        std::wcout << L"Hello. Going to wipe your drive: " << path << std::endl;

        const IO::AutoHandle hdd(IO::OpenW(path));

        if (!hdd.IsValid())
        {
            const uint32_t error = GetLastError();
            std::wcerr << L"Could not open drive: " << error << L" / " << Help::Win32ErrorToString(error) << std::endl;
            return error;
        }

        uint64_t bytesLeft = IO::DiskSize(hdd);

        if (!bytesLeft)
        {
            const uint32_t error = GetLastError();
            std::wcerr << L"Failed to get disk geometry: " << error << L" / " << Help::Win32ErrorToString(error) << std::endl;
            return error;
        }

        std::wcout << L"Bytes to wipe: " << bytesLeft << std::endl;

        uint64_t writtenBytesTotal = 0;
        uint32_t error = ERROR_SUCCESS;
        const Timer<Units::Seconds> stopWatch;

        const auto progress = [](uint64_t bytesWritten, uint64_t secondsElapsed) -> void
        {
            const uint64_t megabytesWritten = bytesWritten / Units::MegaByte;
            std::wcout << megabytesWritten << L" megabytes written. Speed " << megabytesWritten / secondsElapsed << " MB/s" << std::endl;
        };

        if (!IO::Wipe(hdd, bytesLeft, writtenBytesTotal, progress))
        {
            error = GetLastError();
            std::wcerr << L"Write operation failed: " << error << L" / " << Help::Win32ErrorToString(error) << std::endl;
        }

        std::wcout << L"Wiped: " << writtenBytesTotal << L" bytes " << bytesLeft << L" left unwiped" << std::endl;
        std::wcout << L"Took :" << stopWatch << std::endl;

        return error;
    }
}

unsigned long __stdcall TunkioExecuteW(int argc, wchar_t** argv)
{
    using namespace Tunkio;

    if (!Args::Parse({ argv + 1, argv + argc }))
    {
        return ERROR_BAD_ARGUMENTS;
    }

    return Exec(Args::Arguments[0].Value<std::wstring>());
}

unsigned long __stdcall TunkioExecuteA(int /*argc*/, char** /*argv*/)
{
    return ERROR_NOT_SUPPORTED;
}
