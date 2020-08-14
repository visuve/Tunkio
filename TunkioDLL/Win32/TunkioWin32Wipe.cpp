#include "../PCH.hpp"
#include "../TunkioFillStrategy.hpp"
#include "TunkioWin32Wipe.hpp"

namespace Tunkio
{
	Win32Wipe::Win32Wipe(const TunkioOptions* options) :
		IOperation(options)
	{
	}

	Win32Wipe::~Win32Wipe()
	{
	}

	bool Win32Wipe::Open()
	{
		const std::string path(m_options->Path.Data, m_options->Path.Length);
		m_handle.Reset(Open(path));
		return m_handle.IsValid();
	}

	bool Win32Wipe::Fill()
	{
		DWORD bytesWritten = 0u;
		uint64_t bytesLeft = m_size;
		FillStrategy fakeData(m_options->Mode, DataUnit::Mebibyte(1));

		ReportStarted();

		while (bytesLeft)
		{
			if (fakeData.Size<uint64_t>() > bytesLeft)
			{
				fakeData.Resize(bytesLeft);
			}

			const bool result = WriteFile(m_handle.Value(),
				fakeData.Front(),
				fakeData.Size<uint32_t>(),
				&bytesWritten,
				nullptr);

			m_totalBytesWritten += bytesWritten;
			bytesLeft -= bytesWritten;

			if (!result)
			{
				ReportError(GetLastError());
				return false;
			}

			if (!ReportProgress())
			{
				return true;
			}
		}

		ReportComplete();
		return true;
	}

	void Win32Wipe::ReportStarted() const
	{
		m_options->Callbacks.StartedCallback(m_size);
	}

	bool Win32Wipe::ReportProgress() const
	{
		return m_options->Callbacks.ProgressCallback(m_totalBytesWritten);
	}

	void Win32Wipe::ReportComplete() const
	{
		m_options->Callbacks.CompletedCallback(m_totalBytesWritten);
	}

	void Win32Wipe::ReportError(uint32_t error) const
	{
		m_options->Callbacks.ErrorCallback(error, m_totalBytesWritten);
	}

	HANDLE Win32Wipe::Open(const std::string& path)
	{
		// https://docs.microsoft.com/en-us/windows/win32/fileio/file-buffering?redirectedfrom=MSDN
		constexpr uint32_t DesiredAccess = GENERIC_READ | GENERIC_WRITE;
		constexpr uint32_t ShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
		constexpr uint32_t CreationFlags = FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH;

		return CreateFileA(
			path.c_str(),
			DesiredAccess,
			ShareMode,
			nullptr,
			OPEN_EXISTING,
			CreationFlags,
			nullptr);
	}
}