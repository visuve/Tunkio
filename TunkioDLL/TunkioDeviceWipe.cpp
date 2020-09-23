#include "PCH.hpp"
#include "TunkioDeviceWipe.hpp"
#include "TunkioFillGenerator.hpp"
#include "TunkioFile.hpp"

namespace Tunkio
{
	DeviceWipe::DeviceWipe(const TunkioOptions* options) :
		IOperation(options)
	{
	}

	bool DeviceWipe::Run()
	{
		const std::string path(m_options->Path.Data, m_options->Path.Length);

		const File disk(path);

		if (disk.IsValid())
		{
			m_options->Callbacks.ErrorCallback(
				TunkioStage::Open,
				0,
				LastError);

			return false;
		}

		if (disk.Size())
		{
			m_options->Callbacks.ErrorCallback(
				TunkioStage::Size,
				0,
				LastError);

			return false;
		}

		const DataUnit::Mebibyte bufferSize(1);
		uint64_t bytesLeft = disk.Size();
		uint64_t bytesWritten = 0;

		FillGenerator fakeData(m_options->Mode, bufferSize);

		m_options->Callbacks.StartedCallback(bytesLeft);

		while (bytesLeft)
		{
			const uint64_t bytesToWrite = bufferSize > bytesLeft ? bytesLeft : bufferSize.Bytes();
			const auto result = disk.Write(fakeData.Data(), bytesToWrite);

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
		return true;
	}
}