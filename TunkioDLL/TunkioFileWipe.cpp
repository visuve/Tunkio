#include "PCH.hpp"
#include "TunkioFileWipe.hpp"
#include "TunkioFillGenerator.hpp"
#include "TunkioFile.hpp"

namespace Tunkio
{
	FileWipe::FileWipe(const TunkioOptions* options) :
		IOperation(options)
	{
	}

	bool FileWipe::Run()
	{
		const std::filesystem::path path(std::string(m_options->Path.Data, m_options->Path.Length));

		File file(path);

		if (file.IsValid())
		{
			m_options->Callbacks.ErrorCallback(
				TunkioStage::Open,
				0,
				LastError);

			return false;
		}

		if (!file.Size())
		{
			m_options->Callbacks.ErrorCallback(
				TunkioStage::Size,
				0,
				LastError);
		}

		const DataUnit::Mebibyte bufferSize(1);
		uint64_t bytesLeft = file.Size();
		uint64_t bytesWritten = 0;

		FillGenerator fakeData(m_options->Mode, DataUnit::Mebibyte(1));

		m_options->Callbacks.StartedCallback(bytesLeft);

		while (bytesLeft)
		{
			const uint64_t bytesToWrite = bufferSize > bytesLeft ? bytesLeft : bufferSize.Bytes();
			const auto result = file.Write(fakeData.Data(), bytesToWrite);

			bytesWritten += result.second;
			bytesLeft -= result.second;

			if (!result.first)
			{
				m_options->Callbacks.ErrorCallback(
					TunkioStage::Write,
					bytesWritten,
					LastError);

				return false;
			}

			if (!m_options->Callbacks.ProgressCallback(bytesWritten))
			{
				return true;
			}
		}

		m_options->Callbacks.CompletedCallback(bytesWritten);

		if (m_options->Remove && !file.Delete())
		{
			m_options->Callbacks.ErrorCallback(
				TunkioStage::Delete,
				bytesWritten,
				LastError);

			return false;
		}

		return true;
	}
}