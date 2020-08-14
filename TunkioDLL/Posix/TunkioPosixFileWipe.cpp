#include "../PCH.hpp"
#include "../TunkioErrorCodes.hpp"
#include "../TunkioFileWipe.hpp"
#include "TunkioPosixWipe.hpp"

#include <sys/stat.h>

namespace Tunkio
{
	class PosixFileWipe : public PosixWipe
	{
	public:
		PosixFileWipe(const TunkioOptions* options) :
			PosixWipe(options)
		{
		}

		~PosixFileWipe()
		{
		}

		bool Run() override
		{
			if (!Open())
			{
				ReportError(errno);
				return false;
			}

			m_size = Size();

			if (!m_size)
			{
				ReportError(m_error);
				return false;
			}

			return Fill();
		}

		uint64_t Size() override
		{
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

	private:
		uint64_t m_size = 0;
		uint64_t m_totalBytesWritten = 0;
		PosixAutoHandle m_handle;
		int m_error = 0;
	};

	FileWipe::FileWipe(const TunkioOptions* options) :
		IOperation(options),
		m_impl(new PosixFileWipe(options))
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

	uint64_t FileWipe::Size()
	{
		return m_impl->Size();
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