#include "PCH.hpp"
#include "TunkioIO.hpp"
#include "TunkioTimer.hpp"

namespace Tunkio::IO
{
#ifdef TESTING
    extern decltype(CreateFileW) Win32OpenW;
    extern decltype(CreateFileA) Win32OpenA;
    extern decltype(DeviceIoControl) Win32DeviceIoControl;
    extern decltype(WriteFile) Win32Write;
#else
    constexpr auto Win32OpenW = CreateFileW;
    constexpr auto Win32OpenA = CreateFileA;
    constexpr auto Win32DeviceIoControl = DeviceIoControl;
    constexpr auto Win32Write = WriteFile;
#endif
    constexpr uint32_t DesiredAccess = GENERIC_READ | GENERIC_WRITE;
    constexpr uint32_t ShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;

    void* OpenW(const std::wstring& path)
    {
        return Win32OpenW(path.c_str(), DesiredAccess, ShareMode, nullptr, OPEN_EXISTING, 0, nullptr);
    }

    void* OpenA(const std::string& path)
    {
        return Win32OpenA(path.c_str(), DesiredAccess, ShareMode, nullptr, OPEN_EXISTING, 0, nullptr);
    }

    uint64_t DiskSize(const AutoHandle& hdd)
    {
        unsigned long bytesReturned = 0; // Not needed
        DISK_GEOMETRY diskGeo = { 0 };
        constexpr uint32_t diskGeoSize = sizeof(DISK_GEOMETRY);

        if (!Win32DeviceIoControl(hdd, IOCTL_DISK_GET_DRIVE_GEOMETRY, nullptr, 0, &diskGeo, diskGeoSize, &bytesReturned, nullptr))
        {
            return 0u;
        }

        _ASSERT(bytesReturned == sizeof(DISK_GEOMETRY));
        return diskGeo.Cylinders.QuadPart * diskGeo.TracksPerCylinder * diskGeo.SectorsPerTrack * diskGeo.BytesPerSector;
    }

    bool Wipe(const AutoHandle& handle, uint64_t& bytesLeft, uint64_t& writtenBytesTotal, const ProgressCallback& progress)
    {
        const std::vector<char> buffer(Units::MegaByte, '\0');
        Timer<Units::Seconds> timer;

        while (bytesLeft)
        {
            const uint64_t bytesToWrite = bytesLeft < Units::MegaByte ? bytesLeft : Units::MegaByte;
            unsigned long writtenBytes = 0u;

            const bool result = Win32Write(handle, &buffer.front(), static_cast<uint32_t>(bytesToWrite), &writtenBytes, nullptr);
            writtenBytesTotal += writtenBytes;
            bytesLeft -= writtenBytes;

            if (!result)
            {
                std::wcout << L"Wrote only " << writtenBytes << L" of intended " << Units::MegaByte << L" bytes" << std::endl;
                return false;
            }

            if (progress && timer > Units::Seconds(1))
            {
                progress(writtenBytesTotal, timer.Count());
            }
        }

        return true;
    }
}