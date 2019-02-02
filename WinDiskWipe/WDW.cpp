#include "PCH.hpp"
#include "WDW.hpp"

namespace WDW
{
    AutoHandle::AutoHandle(const HANDLE handle) :
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

    AutoHandle::operator const HANDLE() const
    {
        return m_handle;
    }

    UINT64 DiskSize(const AutoHandle & hdd)
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

    bool WipeDrive(const AutoHandle & hdd, UINT64 & bytesLeft, UINT64 & writtenBytesTotal)
    {
        const std::string buffer(MegaByte, '\0');

        while (bytesLeft)
        {
            const DWORD bytesToWrite = bytesLeft < MegaByte ? static_cast<DWORD>(bytesLeft) : MegaByte;
            DWORD writtenBytes = 0;

            const bool result = WriteFile(hdd, &buffer.front(), bytesToWrite, &writtenBytes, nullptr);
            writtenBytesTotal += writtenBytes;
            bytesLeft -= writtenBytes;

            if (!result)
            {
                std::wcout << L"Wrote only " << writtenBytes << L" of intended " << MegaByte << L" bytes" << std::endl;
                return false;
            }

            if (writtenBytesTotal % (MegaByte * 10) == 0)
            {
                std::wcout << writtenBytesTotal / MegaByte << L" megabytes written" << std::endl;
            }
        }

        return true;
    }
}