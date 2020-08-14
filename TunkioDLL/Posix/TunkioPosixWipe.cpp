#include "../PCH.hpp"
#include "../TunkioFillStrategy.hpp"
#include "TunkioPosixWipe.hpp"

namespace Tunkio
{
	PosixWipe::PosixWipe(const TunkioOptions* options) :
		IOperation(options)
	{
	}

	PosixWipe::~PosixWipe()
	{
	}

	bool PosixWipe::Open()
	{
		const std::string path(m_options->Path.Data, m_options->Path.Length);
		m_handle.Reset(Open(path));
		return m_handle.IsValid();
	}

	bool PosixWipe::Fill()
	{
		uint64_t bytesLeft = m_size;
		FillStrategy fakeData(m_options->Mode, DataUnit::Mebibyte(1));

		ReportStarted();

		while (bytesLeft)
		{
			if (fakeData.Size<uint64_t>() > bytesLeft)
			{
				fakeData.Resize(bytesLeft);
			}

			ssize_t bytesWritten =
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

	void PosixWipe::ReportStarted() const
	{
		m_options->Callbacks.StartedCallback(m_size);
	}

	bool PosixWipe::ReportProgress() const
	{
		return m_options->Callbacks.ProgressCallback(m_totalBytesWritten);
	}

	void PosixWipe::ReportComplete() const
	{
		m_options->Callbacks.CompletedCallback(m_totalBytesWritten);
	}

	void PosixWipe::ReportError(uint32_t error) const
	{
		m_options->Callbacks.ErrorCallback(error, m_totalBytesWritten);
	}

	int PosixWipe::Open(const std::string& path)
	{
		// https://linux.die.net/man/2/open
		constexpr uint32_t Flags = O_WRONLY | O_DIRECT | O_LARGEFILE | O_SYNC;
		return open(path.c_str(), Flags);
	}
}