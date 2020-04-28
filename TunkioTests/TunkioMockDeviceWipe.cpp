#include "PCH.hpp"
#include "Strategies/TunkioDeviceWipe.hpp"
#include "Strategies/TunkioFillStrategy.hpp"

namespace Tunkio
{
    class DeviceWipeImpl : IOperation
    {
    public:

        DeviceWipeImpl(const TunkioOptions* options) :
            IOperation(options)
        {
        }

        ~DeviceWipeImpl()
        {
        }

        uint32_t Run() override
        {
            if (!Fill())
            {
                return 666;
            }

            return 0;
        }

        bool Exists() override
        {
            return true;
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

        bool FakeWrite(const uint8_t*, const uint32_t size, uint32_t* written)
        {
            *written = size;
            return true;
        }

        bool Fill() override
        {
            uint32_t bytesWritten = 0u;
            uint64_t bytesLeft = Size();
            FillStrategy fakeData(m_options->Mode, 0x100000);

            while (bytesLeft)
            {
                if (fakeData.Size<uint64_t>() > bytesLeft)
                {
                    fakeData.Resize(bytesLeft);
                }

                const bool result = FakeWrite(fakeData.Front(), fakeData.Size<uint32_t>(), &bytesWritten);
                m_totalBytesWritten += bytesWritten;
                bytesLeft -= bytesWritten;

                if (!result)
                {
                    ReportError(666);
                    return false;
                }

                ReportProgress();
            }

            ReportComplete();
            return true;
        }

    private:
        uint64_t m_size = 0x150000;
        uint64_t m_totalBytesWritten = 0;
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