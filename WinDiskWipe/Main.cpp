#include "PCH.hpp"
#include "StopWatch.hpp"

namespace
{
    constexpr DWORD KiloByte = 1024;
    constexpr DWORD MegaByte = KiloByte * 1024;

    class AutoHandle
    {
    public:
        AutoHandle(const HANDLE handle) :
            m_handle(handle)
        {
        }

        ~AutoHandle()
        {
            if (m_handle)
            {
                CloseHandle(m_handle);
                m_handle = nullptr;
            }
        }

        bool IsValid() const
        {
            return m_handle != INVALID_HANDLE_VALUE;
        }

        operator const HANDLE() const
        {
            return m_handle;
        }

    private:
        HANDLE m_handle = INVALID_HANDLE_VALUE;
    };

    UINT64 DiskSize(const AutoHandle& hdd)
    {
        DWORD bytesReturned = 0; // Not needed
        DISK_GEOMETRY diskGeo = { 0 };
        constexpr DWORD diskGeoSize = sizeof(DISK_GEOMETRY);

        if (!DeviceIoControl(hdd, IOCTL_DISK_GET_DRIVE_GEOMETRY, nullptr, 0, &diskGeo, diskGeoSize, &bytesReturned, nullptr))
        {
            return 0;
        }

        _ASSERT(bytesReturned == sizeof(DISK_GEOMETRY));
        return diskGeo.Cylinders.QuadPart * diskGeo.TracksPerCylinder * diskGeo.SectorsPerTrack * diskGeo.BytesPerSector;
    }

    std::wstring Win32ErrorToString(const DWORD error)
    {
        constexpr DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
        constexpr DWORD langId = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
        wchar_t buffer[KiloByte] = { 0 };
        const DWORD size = FormatMessage(flags, nullptr, error, langId, buffer, KiloByte, nullptr);
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
        PrintUsage();
        return ERROR_BAD_ARGUMENTS;
    }

    std::wcout << L"Hello. Going to wipe your drive: " << argv[1] << std::endl;

    constexpr DWORD desiredAccess = GENERIC_READ | GENERIC_WRITE;
    constexpr DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
    const AutoHandle hdd = CreateFile(argv[1], desiredAccess, shareMode, nullptr, OPEN_EXISTING, 0, nullptr);

    if (!hdd.IsValid())
    {
        const DWORD error = GetLastError();
        std::wcerr << L"Could not open drive: " << error << L" / " << Win32ErrorToString(error) << std::endl;
        return error;
    }

    UINT64 bytesLeft = DiskSize(hdd);

    if (!bytesLeft)
    {
        const DWORD error = GetLastError();
        std::wcerr << L"Failed to get disk geometry: " << error << L" / " << Win32ErrorToString(error) << std::endl;
        return error;
    }

    std::wcout << L"Bytes to wipe: " << bytesLeft << std::endl;

    const std::string buffer(MegaByte, '\0');
    UINT64 writtenBytesTotal = 0;
    const StopWatch<std::chrono::seconds> stopWatch;

    while (bytesLeft)
    {
        const DWORD bytesToWrite = bytesLeft < MegaByte ? static_cast<DWORD>(bytesLeft) : MegaByte;
        DWORD writtenBytes = 0;

        const bool result = WriteFile(hdd, &buffer.front(), bytesToWrite, &writtenBytes, nullptr);
        writtenBytesTotal += writtenBytes;
        bytesLeft -= writtenBytes;

        if (!result)
        {
            const DWORD error = GetLastError();
            std::wcerr << L"Write operation failed: " << error << L" / " << Win32ErrorToString(error) << std::endl;
            std::wcout << L"Wrote only " << writtenBytes << L" of intended " << MegaByte << L" bytes" << std::endl;
            std::wcout << L"Wiped: " << writtenBytesTotal << L" bytes " << bytesLeft << L" left unwiped" << std::endl;
            std::wcout << L"Took :" << stopWatch << std::endl;
            return error;
        }

        if (writtenBytesTotal % (MegaByte * 10) == 0)
        {
            std::wcout << writtenBytesTotal / MegaByte << L" megabytes written" << std::endl;
        }
    }

    std::wcout << L"Wiped: " << writtenBytesTotal << L" bytes " << bytesLeft << L" left unwiped" << std::endl;
    std::wcout << L"Took :" << stopWatch << std::endl;

    return 0;
}