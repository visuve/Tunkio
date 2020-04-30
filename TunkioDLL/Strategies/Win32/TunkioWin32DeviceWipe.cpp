#include "PCH.hpp"
#include "TunkioErrorCodes.hpp"
#include "TunkioWin32AutoHandle.hpp"
#include "Strategies/TunkioDeviceWipe.hpp"
#include "Strategies/TunkioFillStrategy.hpp"

namespace Tunkio
{
    // https://docs.microsoft.com/en-us/windows/win32/fileio/file-buffering?redirectedfrom=MSDN
    constexpr uint32_t DesiredAccess = GENERIC_READ | GENERIC_WRITE;
    constexpr uint32_t ShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
    constexpr uint32_t CreationFlags = FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH;

    class DeviceWipeImpl : IOperation
    {
    public:

        DeviceWipeImpl(const TunkioOptions* options) :
            IOperation(options),
            m_handle(CreateFileA(options->Path.Data, DesiredAccess, ShareMode, nullptr, OPEN_EXISTING, CreationFlags, nullptr))
        {
            if (m_handle.IsValid())
            {
                unsigned long bytesReturned = 0; // Not needed
                DISK_GEOMETRY diskGeo = { 0 };
                constexpr uint32_t diskGeoSize = sizeof(DISK_GEOMETRY);

                if (DeviceIoControl(m_handle.Handle(), IOCTL_DISK_GET_DRIVE_GEOMETRY, nullptr, 0, &diskGeo, diskGeoSize, &bytesReturned, nullptr))
                {
                    _ASSERT(bytesReturned == sizeof(DISK_GEOMETRY));
                    m_size = diskGeo.Cylinders.QuadPart * diskGeo.TracksPerCylinder * diskGeo.SectorsPerTrack * diskGeo.BytesPerSector;
                }
            }
        }

        ~DeviceWipeImpl()
        {
            // Will not work, because the handle is open...
            //if (m_handle.IsValid() && m_removeOnExit && !DeleteFileA(m_path.c_str()))
            //{
            //    ReportError(ErrorCode::RemoveFailed);
            //}
        }

        uint32_t Run() override
        {
            if (!Fill())
            {
                return GetLastError();
            }

            return 0; // TODO: ...
        }

        bool Exists() override
        {
            return m_handle.IsValid();
        }

        uint64_t Size() override
        {
            return m_size;
        }

        bool Remove() override
        {
            return false;
        }

        void ReportProgress() const
        {
            m_options->Callbacks.ProgressCallback(m_totalBytesWritten);
        }

        void ReportComplete() const
        {
            m_options->Callbacks.CompletedCallback(m_totalBytesWritten);
        }

        void ReportError(uint32_t error) const
        {
            m_options->Callbacks.ErrorCallback(error, m_totalBytesWritten);
        }

        bool Fill() override
        {
            DWORD bytesWritten = 0u;
            uint64_t bytesLeft = Size();
            FillStrategy fakeData(m_options->Mode, DataUnit::Mebibyte(10));

            while (bytesLeft)
            {
                if (fakeData.Size<uint64_t>() > bytesLeft)
                {
                    fakeData.Resize(bytesLeft);
                }

                const bool result = WriteFile(m_handle.Handle(), fakeData.Front(), fakeData.Size<uint32_t>(), &bytesWritten, nullptr);
                m_totalBytesWritten += bytesWritten;
                bytesLeft -= bytesWritten;

                if (!result)
                {
                    ReportError(GetLastError());
                    return false;
                }

                ReportProgress();
            }

            ReportComplete();
            return true;
        }

    private:
        uint64_t m_size = 0;
        uint64_t m_totalBytesWritten = 0;
        Win32AutoHandle m_handle;
    };

    DeviceWipe::DeviceWipe(const TunkioOptions* options) :
        IOperation(options),
        m_impl(new DeviceWipeImpl(options))
    {
    }

    DeviceWipe::~DeviceWipe()
    {
        delete m_impl;
    }

    uint32_t DeviceWipe::Run()
    {
        return m_impl->Run();
    }

    bool DeviceWipe::Exists()
    {
        return m_impl->Exists();
    }

    uint64_t DeviceWipe::Size()
    {
        return m_impl->Size();
    }

    bool DeviceWipe::Fill()
    {
        return m_impl->Fill();
    }

    bool DeviceWipe::Remove()
    {
        return m_impl->Remove();
    }
}