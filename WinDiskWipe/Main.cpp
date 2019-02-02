#include "PCH.hpp"
#include "WDW.hpp"
#include "Timer.hpp"

namespace Help
{
    std::wstring Win32ErrorToString(const DWORD error)
    {
        constexpr DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
        constexpr DWORD langId = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
        wchar_t buffer[WDW::KiloByte] = { 0 };
        const DWORD size = FormatMessage(flags, nullptr, error, langId, buffer, WDW::KiloByte, nullptr);
        return std::wstring(buffer, size);
    }

    void PrintUsage()
    {
        std::wcerr << L"Please provide path to the drive to wipe." << std::endl;
        std::wcout << L"Example: \\\\.\\PHYSICALDRIVE1" << std::endl;
        std::wcout << std::endl << "Here are your drives:" << std::endl << std::endl;

        DWORD error = static_cast<DWORD>(_wsystem(L"wmic diskdrive list brief"));

        if (error != 0)
        {
            std::wcerr << L"Listing your drives failed with wmic: " << error << std::endl;
        }
    }
}

int wmain(int argc, wchar_t* argv[])
{
    if (argc <= 1)
    {
        Help::PrintUsage();
        return ERROR_BAD_ARGUMENTS;
    }

    std::wcout << L"Hello. Going to wipe your drive: " << argv[1] << std::endl;

    constexpr DWORD desiredAccess = GENERIC_READ | GENERIC_WRITE;
    constexpr DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
    const WDW::AutoHandle hdd = CreateFile(argv[1], desiredAccess, shareMode, nullptr, OPEN_EXISTING, 0, nullptr);

    if (!hdd.IsValid())
    {
        const DWORD error = GetLastError();
        std::wcerr << L"Could not open drive: " << error << L" / " << Help::Win32ErrorToString(error) << std::endl;
        return error;
    }

    UINT64 bytesLeft = DiskSize(hdd);

    if (!bytesLeft)
    {
        const DWORD error = GetLastError();
        std::wcerr << L"Failed to get disk geometry: " << error << L" / " << Help::Win32ErrorToString(error) << std::endl;
        return error;
    }

    std::wcout << L"Bytes to wipe: " << bytesLeft << std::endl;

    UINT64 writtenBytesTotal = 0;
    DWORD error = ERROR_SUCCESS;
    const Timer<std::chrono::seconds> stopWatch;

    const auto progress = [](UINT64 bytes, std::chrono::seconds time) -> void
    {
        const UINT64 megabytes = bytes / WDW::MegaByte;
        std::wcout << megabytes << L" megabytes written. Speed " << megabytes / time.count() << " MB/s" << std::endl;
    };

    if (!WipeDrive(hdd, bytesLeft, writtenBytesTotal, progress))
    {
        error = GetLastError();
        std::wcerr << L"Write operation failed: " << error << L" / " << Help::Win32ErrorToString(error) << std::endl;
    }

    std::wcout << L"Wiped: " << writtenBytesTotal << L" bytes " << bytesLeft << L" left unwiped" << std::endl;
    std::wcout << L"Took :" << stopWatch << std::endl;

    return error;
}