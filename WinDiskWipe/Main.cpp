#include "PCH.hpp"
#include "StopWatch.hpp"

namespace
{
    constexpr DWORD KiloByte = 1024;
    constexpr DWORD MegaByte = KiloByte * 1024;

    class AutoHandle
    {
    public:
        AutoHandle(HANDLE handle) :
            m_handle(handle)
        {
        }

        ~AutoHandle()
        {
            Close();
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
        void Close()
        {
            if (m_handle)
            {
                CloseHandle(m_handle);
                m_handle = nullptr;
            }
        }

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

    std::wstring Win32ErrorToString(DWORD error)
    {
        wchar_t buffer[KiloByte] = { 0 };
        constexpr DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
        constexpr DWORD langId = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
        DWORD size = FormatMessage(flags, nullptr, error, langId, buffer, KiloByte, nullptr);
        return std::wstring(buffer, size);
    }
}

int wmain(int argc, wchar_t* argv[])
{
    if (argc <= 1)
    {
        std::wcerr << L"Please provide path to the drive to nuke." << std::endl;
        std::wcout << L"Example: \\\\.\\PHYSICALDRIVE1" << std::endl;
        return ERROR_BAD_ARGUMENTS;
    }

    std::wcout << L"Hello. Going to wipe your drive: " << argv[1] << std::endl;

    constexpr DWORD desiredAccess = GENERIC_READ | GENERIC_WRITE;
    constexpr DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
    AutoHandle hdd = CreateFile(argv[1], desiredAccess, shareMode, nullptr, OPEN_EXISTING, 0, nullptr);

    if (!hdd.IsValid())
    {
        DWORD error = GetLastError();
        std::wcerr << L"Could not open HDD: " << error << L" / " << Win32ErrorToString(error) << std::endl;
        return error;
    }

    UINT64 bytesLeft = DiskSize(hdd);

    if (!bytesLeft)
    {
        DWORD error = GetLastError();
        std::wcerr << L"Failed to get disk geometry: " << Win32ErrorToString(error) << std::endl;
        return error;
    }

    std::wcout << L"Bytes to nuke: " << bytesLeft << std::endl;

    const std::string buffer(MegaByte, '\0');
    UINT64 writtenBytesTotal = 0;
    StopWatch<std::chrono::seconds> stopWatch;

    while (bytesLeft)
    {
        DWORD writtenBytes = 0;
        const DWORD bytesToWrite = bytesLeft < MegaByte ? static_cast<DWORD>(bytesLeft) : MegaByte;

        if (!WriteFile(hdd, &buffer.front(), bytesToWrite, &writtenBytes, nullptr))
        {
            DWORD error = GetLastError();
            std::wcerr << L"Write failed: " << error << L" / " << Win32ErrorToString(error) << std::endl;
            std::wcout << L"Could write only " << writtenBytes << L" of " << MegaByte << L" bytes" << std::endl;
            break;
        }

        writtenBytesTotal += writtenBytes;
        bytesLeft -= writtenBytes;

        if (writtenBytesTotal % (MegaByte * 10) == 0)
        {
            std::wcout << writtenBytesTotal / MegaByte << L" megabytes written" << std::endl;
        }
    }

    std::wcout << L"Nuked: " << writtenBytesTotal << L" bytes " << bytesLeft << L"left unnuked" << std::endl;
    std::wcout << L"Took :" << stopWatch << std::endl;

    return 0;
}