#include "../PCH.hpp"
#include "../TunkioErrorCodes.hpp"
#include "../TunkioFileWipe.hpp"
#include "../TunkioFillStrategy.hpp"
#include "TunkioPosixAutoHandle.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>  
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysmacros.h>

namespace Tunkio
{
	constexpr uint32_t Flags = O_WRONLY | O_DIRECT | O_LARGEFILE | O_SYNC;

	class FileWipeImpl : IOperation
	{
	public:

		FileWipeImpl(const TunkioOptions* options) :
			IOperation(options)
		{
		}

		~FileWipeImpl()
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

			struct ::stat64 buffer = { 0 };
			m_error = ::fstat64(m_handle.Value(), &buffer);

			if (m_error != 0)
			{
				return false;
			}

			m_size = buffer.st_size;

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
					write(m_handle.Value(), fakeData.Front(), fakeData.Size<size_t>());

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

	bool FileWipe::Open()
	{
		return m_impl->Open();
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