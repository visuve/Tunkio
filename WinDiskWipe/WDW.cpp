#include "PCH.hpp"
#include "WDW.hpp"
#include "Timer.hpp"

namespace WDW
{
    AutoHandle::AutoHandle(const RawHandle handle) :
        m_handle(handle)
    {
    }

    AutoHandle::~AutoHandle()
    {
        if (m_handle)
        {
            CloseHandle(m_handle);
            m_handle = nullptr;
        }
    }

    bool AutoHandle::IsValid() const
    {
        return m_handle != INVALID_HANDLE_VALUE;
    }

    AutoHandle::operator const RawHandle() const
    {
        return m_handle;
    }

    uint64_t DiskSize(const AutoHandle& hdd)
    {
        unsigned long bytesReturned = 0; // Not needed
        DISK_GEOMETRY diskGeo = { 0 };
        constexpr uint32_t diskGeoSize = sizeof(DISK_GEOMETRY);

        if (!DeviceIoControl(hdd, IOCTL_DISK_GET_DRIVE_GEOMETRY, nullptr, 0, &diskGeo, diskGeoSize, &bytesReturned, nullptr))
        {
            return 0u;
        }

        _ASSERT(bytesReturned == sizeof(DISK_GEOMETRY));
        return diskGeo.Cylinders.QuadPart * diskGeo.TracksPerCylinder * diskGeo.SectorsPerTrack * diskGeo.BytesPerSector;
    }

    bool WipeDrive(const AutoHandle& hdd, uint64_t& bytesLeft, uint64_t& writtenBytesTotal, const ProgressCallback& progress)
    {
        const std::string buffer(Units::MegaByte, '\0');
        Timer<Units::Seconds> timer;

        while (bytesLeft)
        {
            const uint64_t bytesToWrite = bytesLeft < Units::MegaByte ? bytesLeft : Units::MegaByte;
            unsigned long writtenBytes = 0u;

            const bool result = WriteFile(hdd, &buffer.front(), static_cast<uint32_t>(bytesToWrite), &writtenBytes, nullptr);
            writtenBytesTotal += writtenBytes;
            bytesLeft -= writtenBytes;

            if (!result)
            {
                std::wcout << L"Wrote only " << writtenBytes << L" of intended " << Units::MegaByte << L" bytes" << std::endl;
                return false;
            }

            if (timer > Units::Seconds(1))
            {
                progress(writtenBytesTotal, timer.Count());
            }
        }

        return true;
    }
}