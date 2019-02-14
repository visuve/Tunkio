#include "PCH.hpp"
#include "TunkioNative.hpp"
#include "TunkioTiming.hpp"

namespace Tunkio::Native
{
    namespace Win32
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

        std::wstring ErrorToString(const uint32_t error)
        {
            constexpr uint32_t flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
            constexpr uint32_t langId = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
            wchar_t buffer[1024] = { 0 };
            const size_t size = Win32FormatMessage(flags, nullptr, error, langId, buffer, 1024, nullptr);
            return std::wstring(buffer, size);
        }

        RawHandle Open(const Path& path)
        {
            constexpr uint32_t DesiredAccess = GENERIC_READ | GENERIC_WRITE;
            constexpr uint32_t ShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
            return Win32CreateFile(path.c_str(), DesiredAccess, ShareMode, nullptr, OPEN_EXISTING, 0, nullptr);
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

        bool Fill(const AutoHandle& handle, uint64_t& bytesLeft, uint64_t& writtenBytesTotal, TunkioProgressCallback progress)
        {
            const std::vector<uint8_t> fakeData(1024, '\0');
            Timing::Timer timer;

            while (bytesLeft)
            {
                const uint64_t bytesToWrite = bytesLeft < fakeData.size() ? bytesLeft : fakeData.size();
                unsigned long writtenBytes = 0u;

                // TODO: make async with WriteFileEx
                const bool result = Win32WriteFile(handle, &fakeData.front(), static_cast<uint32_t>(bytesToWrite), &writtenBytes, nullptr);
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