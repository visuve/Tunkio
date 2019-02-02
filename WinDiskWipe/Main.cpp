#include "PCH.hpp"
#include "WDW.hpp"
#include "Timer.hpp"

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

    void PrintUsage()
    {
        std::wcerr << L"Please provide path to the drive to wipe." << std::endl;
        std::wcout << L"Example: \\\\.\\PHYSICALDRIVE1" << std::endl;
        std::wcout << std::endl << "Here are your drives:" << std::endl << std::endl;

        int error = _wsystem(L"wmic diskdrive list brief");

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

    constexpr uint32_t desiredAccess = GENERIC_READ | GENERIC_WRITE;
    constexpr uint32_t shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
    const WDW::AutoHandle hdd = CreateFile(argv[1], desiredAccess, shareMode, nullptr, OPEN_EXISTING, 0, nullptr);

    if (!hdd.IsValid())
    {
        const uint32_t error = GetLastError();
        std::wcerr << L"Could not open drive: " << error << L" / " << Help::Win32ErrorToString(error) << std::endl;
        return error;
    }

    uint64_t bytesLeft = DiskSize(hdd);

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

    if (!WipeDrive(hdd, bytesLeft, writtenBytesTotal, progress))
    {
        error = GetLastError();
        std::wcerr << L"Write operation failed: " << error << L" / " << Help::Win32ErrorToString(error) << std::endl;
    }

    std::wcout << L"Wiped: " << writtenBytesTotal << L" bytes " << bytesLeft << L" left unwiped" << std::endl;
    std::wcout << L"Took :" << stopWatch << std::endl;

    return error;
}