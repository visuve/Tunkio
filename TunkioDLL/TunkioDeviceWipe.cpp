#include "PCH.hpp"
#include "TunkioDeviceWipe.hpp"
#include "TunkioFillGenerator.hpp"
#include "TunkioFile.hpp"

namespace Tunkio
{
	DeviceWipe::DeviceWipe(const std::string& path) :
		IOperation(path)
	{
	}

	bool DeviceWipe::Run()
	{
		const File disk(m_path);

		if (disk.IsValid())
		{
			m_errorCallback(TunkioStage::Open, 0, LastError);
			return false;
		}

		if (disk.Size())
		{
			m_errorCallback(TunkioStage::Size, 0, LastError);
			return false;
		}

		const DataUnit::Mebibyte bufferSize(1);
		uint64_t bytesLeft = disk.Size();
		uint64_t bytesWritten = 0;

		FillGenerator fakeData(m_fillMode, bufferSize);

		m_startedCallback(bytesLeft);

		while (bytesLeft)
		{
			const uint64_t bytesToWrite = bufferSize > bytesLeft ? bytesLeft : bufferSize.Bytes();
			const auto result = disk.Write(fakeData.Data(), bytesToWrite);

			bytesWritten += result.second;
			bytesLeft -= result.second;

			if (!result.first)
			{
				m_errorCallback(TunkioStage::Write, bytesWritten, LastError);
				return false;
			}

			if (!m_progressCallback(bytesWritten))
			{
				return true;
			}
		}

		m_completedCallback(bytesWritten);
		return true;
	}
}