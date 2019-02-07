#include "PCH.hpp"
#include "WDW.hpp"
#include "Timer.hpp"

namespace WDW
{
#ifdef TESTING
    extern decltype(DeviceIoControl) Win32DeviceIoControl;
    extern decltype(WriteFile) Win32Write;
#else
    constexpr auto Win32DeviceIoControl = DeviceIoControl;
    constexpr auto Win32Write = WriteFile;
#endif

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