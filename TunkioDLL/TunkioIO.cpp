#include "PCH.hpp"
#include "TunkioIO.hpp"
#include "TunkioUnits.hpp"
#include "TunkioTiming.hpp"

namespace Tunkio::IO
{
#ifdef TESTING
    extern decltype(CreateFileW) Win32Open;
    extern decltype(GetFileSizeEx) Win32FileSize;
    extern decltype(DeviceIoControl) Win32DeviceIoControl;
    extern decltype(WriteFile) Win32Write;
    extern decltype(DeleteFileW) Win32DeleteFile;
#else
    constexpr auto Win32Open = CreateFileW;
    constexpr auto Win32FileSize = GetFileSizeEx;
    constexpr auto Win32DeviceIoControl = DeviceIoControl;
    constexpr auto Win32Write = WriteFile;
    constexpr auto Win32DeleteFile = DeleteFileW;
#endif

    RawHandle Open(const std::wstring& path)
    {
        constexpr uint32_t DesiredAccess = GENERIC_READ | GENERIC_WRITE;
        constexpr uint32_t ShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
        return Win32Open(path.c_str(), DesiredAccess, ShareMode, nullptr, OPEN_EXISTING, 0, nullptr);
    }

    uint64_t FileSize(const AutoHandle & file)
    {
        LARGE_INTEGER fileSize = { 0 };

        if (!Win32FileSize(file, &fileSize))
        {
            return 0u;
        }

        return fileSize.QuadPart;
    }

    uint64_t VolumeSize(const AutoHandle& volume)
    {
        unsigned long bytesReturned = 0; // Not needed
        DISK_GEOMETRY diskGeo = { 0 };
        constexpr uint32_t diskGeoSize = sizeof(DISK_GEOMETRY);

        if (!Win32DeviceIoControl(volume, IOCTL_DISK_GET_DRIVE_GEOMETRY, nullptr, 0, &diskGeo, diskGeoSize, &bytesReturned, nullptr))
        {
            return 0u;
        }

        _ASSERT(bytesReturned == sizeof(DISK_GEOMETRY));
        return diskGeo.Cylinders.QuadPart * diskGeo.TracksPerCylinder * diskGeo.SectorsPerTrack * diskGeo.BytesPerSector;
    }

    bool Wipe(const AutoHandle& handle, uint64_t& bytesLeft, uint64_t& writtenBytesTotal, const ProgressCallback& progress)
    {
        const std::vector<char> buffer(Units::MegaByte, '\0');
        Timing::Timer timer;

        while (bytesLeft)
        {
            const uint64_t bytesToWrite = bytesLeft < Units::MegaByte ? bytesLeft : Units::MegaByte;
            unsigned long writtenBytes = 0u;

            // TODO: make async with WriteFileEx
            const bool result = Win32Write(handle, &buffer.front(), static_cast<uint32_t>(bytesToWrite), &writtenBytes, nullptr);
            writtenBytesTotal += writtenBytes;
            bytesLeft -= writtenBytes;

            if (!result)
            {
                return false;
            }

            if (progress && timer.Elapsed<Timing::Seconds>() > Timing::Seconds(1))
            {
                progress(writtenBytesTotal, timer.Elapsed<Timing::Seconds>().count());
            }
        }

        return true;
    }

    bool RemoveFile(const std::wstring& path)
    {
        return Win32DeleteFile(path.c_str());
    }
}