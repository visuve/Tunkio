#include "PCH.hpp"
#include "TunkioDeviceWipe.hpp"
#include "TunkioFillStrategy.hpp"
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

		uint64_t bytesLeft = disk.Size();
		uint64_t bytesWritten = 0;

		FillStrategy fakeData(m_options->Mode, DataUnit::Mebibyte(1));

		m_options->Callbacks.StartedCallback(bytesLeft);

		while (bytesLeft)
		{
			if (fakeData.Size<uint64_t>() > bytesLeft)
			{
				fakeData.Resize(bytesLeft);
			}

			const auto result = disk.Write(fakeData.Front(), fakeData.Size<uint32_t>());

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