#include "PCH.hpp"
#include "TunkioIO.hpp"
#include "TunkioUnits.hpp"
#include "TunkioTiming.hpp"

namespace Tunkio::IO
{
    namespace File
    {
        FileStream Open(const Path& file)
        {
            return FileStream(file, std::ios::in | std::ios::out | std::ios::binary);
        }

        uint64_t Size(const Path& file)
        {
            return FileSystem::file_size(file);
        }

        bool Fill(FileStream& file, uint64_t& bytesLeft, uint64_t& writtenBytesTotal, const ProgressCallback& progress)
        {
            const std::vector<uint8_t> buffer(Units::MegaByte, 0);
            Timing::Timer timer;

            const auto start = file.tellp();

            StreamBuffer* stream = file.rdbuf();

            while (bytesLeft)
            {
                const uint64_t bytesToWrite = bytesLeft < Units::MegaByte ? bytesLeft : Units::MegaByte;
                const char* data = reinterpret_cast<const char*>(&buffer.front());
                const uint64_t writtenBytes = stream->sputn(data, bytesToWrite);
                bytesLeft -= writtenBytes;
                writtenBytesTotal += writtenBytes;

                if (!writtenBytes)
                {
                    return false;
                }

                if (progress && timer.Elapsed<Timing::Seconds>() > Timing::Seconds(1))
                {
                    progress(writtenBytesTotal, timer.Elapsed<Timing::Seconds>().count());
                }

            }

            return bytesLeft == 0 && writtenBytesTotal > 0;
        }

        bool Remove(const Path& path)
        {
            return FileSystem::remove(path);
        }
    }

    namespace Volume
    {
        RawHandle Open(const std::wstring& path)
        {
            constexpr uint32_t DesiredAccess = GENERIC_READ | GENERIC_WRITE;
            constexpr uint32_t ShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
            return Win32Open(path.c_str(), DesiredAccess, ShareMode, nullptr, OPEN_EXISTING, 0, nullptr);
        }

        uint64_t Size(const AutoHandle& volume)
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

        bool Fill(const AutoHandle& handle, uint64_t& bytesLeft, uint64_t& writtenBytesTotal, const ProgressCallback& progress)
        {
            const std::vector<uint8_t> fakeData(Units::MegaByte, '\0');
            Timing::Timer timer;

            while (bytesLeft)
            {
                const uint64_t bytesToWrite = bytesLeft < Units::MegaByte ? bytesLeft : Units::MegaByte;
                unsigned long writtenBytes = 0u;

                // TODO: make async with WriteFileEx
                const bool result = Win32Write(handle, &fakeData.front(), static_cast<uint32_t>(bytesToWrite), &writtenBytes, nullptr);
                writtenBytesTotal += writtenBytes;
                bytesLeft -= writtenBytes;

                if (!result || !writtenBytes)
                {
                    return false;
                }

                if (progress && timer.Elapsed<Timing::Seconds>() > Timing::Seconds(1))
                {
                    progress(writtenBytesTotal, timer.Elapsed<Timing::Seconds>().count());
                }
            }

            return bytesLeft == 0 && writtenBytesTotal > 0;
        }
    }
}