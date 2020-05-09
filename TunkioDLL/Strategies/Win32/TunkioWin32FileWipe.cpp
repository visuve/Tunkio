#include "PCH.hpp"
#include "TunkioErrorCodes.hpp"
#include "TunkioWin32AutoHandle.hpp"
#include "Strategies/TunkioFileWipe.hpp"
#include "Strategies/TunkioFillStrategy.hpp"

namespace Tunkio
{
    // https://docs.microsoft.com/en-us/windows/win32/fileio/file-buffering?redirectedfrom=MSDN
    constexpr uint32_t DesiredAccess = GENERIC_READ | GENERIC_WRITE;
    constexpr uint32_t ShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
    constexpr uint32_t CreationFlags = FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH;

    class FileWipeImpl : IOperation
    {
    public:

        FileWipeImpl(const TunkioOptions* options) :
            IOperation(options),
            m_handle(CreateFileA(options->Path.Data, DesiredAccess, ShareMode, nullptr, OPEN_EXISTING, CreationFlags, nullptr))
        {
            if (m_handle.IsValid())
            {
                LARGE_INTEGER fileSize = { 0 };

                if (GetFileSizeEx(m_handle.Handle(), &fileSize))
                {
                    m_size = fileSize.QuadPart;
                }
            }
        }

        ~FileWipeImpl()
        {
            // Will not work, because the handle is open...
            //if (m_handle.IsValid() && m_removeOnExit && !DeleteFileA(m_path.c_str()))
            //{
            //    ReportError(ErrorCode::RemoveFailed);
            //}
        }

        bool Run()
        {
            if (!m_handle.IsValid())
            {
                ReportError(ErrorCode::FileNotFound);
                return false;
            }

            if (!m_size)
            {
                ReportError(ErrorCode::FileEmpty);
                return false;
            }

            if (!Fill())
            {
                ReportError(GetLastError());
                return false;
            }

            return true;
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
            uint64_t bytesLeft = m_size;
            FillStrategy fakeData(m_options->Mode, DataUnit::Mebibyte(1));

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

    FileWipe::FileWipe(const TunkioOptions* options) :
        IOperation(options),
        m_impl(new FileWipeImpl(options))
    {
    }

    FileWipe::~FileWipe()
    {
        delete m_impl;
    }

    bool FileWipe::Run()
    {
        return m_impl->Run();
    }

    bool FileWipe::Fill()
    {
        return m_impl->Fill();
    }

    bool FileWipe::Remove()
    {
        return m_impl->Remove();
    }
}