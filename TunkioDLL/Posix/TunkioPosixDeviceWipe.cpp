#include "../PCH.hpp"
#include "../TunkioErrorCodes.hpp"
#include "../TunkioFillStrategy.hpp"
#include "../TunkioDeviceWipe.hpp"
#include "TunkioPosixAutoHandle.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/fs.h>
#include <sys/ioctl.h>
#include <stdio.h>

namespace Tunkio
{
	constexpr uint32_t Flags = O_WRONLY | O_DIRECT | O_LARGEFILE | O_SYNC;

	// https://linux.die.net/man/2/open
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

		bool Run() override
		{
			if (!Open())
			{
				ReportError(errno);
				return false;
			}

			if (!m_size)
			{
				ReportError(m_error);
				return false;
			}

			return Fill();
		}

		bool Open() override
		{
			const std::string path(m_options->Path.Data, m_options->Path.Length);
			m_handle.Reset(open(path.c_str(), Flags));

			if (!m_handle.IsValid())
			{
				return false;
			}

			m_error = ioctl(m_handle.Descriptor(), BLKGETSIZE64, &m_size);

			if (m_error != 0)
			{
				return false;
			}

			return true;

		}

		bool Remove() override
		{
			return false;
		}

		void ReportStarted() const
		{
			m_options->Callbacks.StartedCallback(m_size);
		}

		bool ReportProgress() const
		{
			return m_options->Callbacks.ProgressCallback(m_totalBytesWritten);
		}

		void ReportError(uint32_t error) const
		{
			m_options->Callbacks.ErrorCallback(error, m_totalBytesWritten);
		}

		void ReportComplete() const
		{
			m_options->Callbacks.CompletedCallback(m_totalBytesWritten);
		}

		bool Fill() override
		{
			size_t bytesWritten = 0u;
			uint64_t bytesLeft = m_size;
			FillStrategy fakeData(m_options->Mode, DataUnit::Mebibyte(1));

			ReportStarted();

			while (bytesLeft)
			{
				if (fakeData.Size<uint64_t>() > bytesLeft)
				{
					fakeData.Resize(bytesLeft);
				}

				bytesWritten = 
					write(m_handle.Descriptor(), fakeData.Front(), fakeData.Size<size_t>());

				if (!bytesWritten)
				{
					ReportError(errno);
					return false;
				}

				m_totalBytesWritten += bytesWritten;
				bytesLeft -= bytesWritten;

				if (!ReportProgress())
				{
					return true;
				}
			}

			ReportComplete();
			return true;
		}

	private:
		uint64_t m_size = 0;
		uint64_t m_totalBytesWritten = 0;
		PosixAutoHandle m_handle;
		int m_error = 0;
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

	bool DeviceWipe::Run()
	{
		return m_impl->Run();
	}

	bool DeviceWipe::Open()
	{
		return m_impl->Open();
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