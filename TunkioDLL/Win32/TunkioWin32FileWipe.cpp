#include "../PCH.hpp"
#include "../TunkioErrorCodes.hpp"
#include "../TunkioFileWipe.hpp"
#include "TunkioWin32Wipe.hpp"

namespace Tunkio
{
	class Win32FileWipe : public Win32Wipe
	{
	public:
		Win32FileWipe(const TunkioOptions* options) :
			Win32Wipe(options)
		{
		}

		~Win32FileWipe()
		{
		}

		bool Run() override
		{
			if (!Open())
			{
				ReportError(GetLastError());
				return false;
			}

			m_size = Size();

			if (!m_size)
			{
				ReportError(ErrorCode::NoData);
				return false;
			}

			return Win32Wipe::Fill();
		}

		uint64_t Size() override
		{
			if (!m_handle.IsValid())
			{
				return false;
			}

			LARGE_INTEGER fileSize = { 0 };

			if (!GetFileSizeEx(m_handle.Value(), &fileSize))
			{
				return false;
			}

			return fileSize.QuadPart;
		}

		bool Remove() override
		{
			const std::string path(m_options->Path.Data, m_options->Path.Length);
			m_handle.Reset();
			return DeleteFileA(path.c_str());
		}

	private:
		uint64_t m_size = 0;
		uint64_t m_totalBytesWritten = 0;
		Win32AutoHandle m_handle;
	};

	FileWipe::FileWipe(const TunkioOptions* options) :
		IOperation(options),
		m_impl(new Win32FileWipe(options))
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