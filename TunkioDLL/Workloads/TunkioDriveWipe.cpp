#include "../PCH.hpp"
#include "TunkioDriveWipe.hpp"
#include "../TunkioFillGenerator.hpp"
#include "../TunkioFile.hpp"

namespace Tunkio
{
	DriveWipe::DriveWipe(const std::string& path) :
		IOperation(path)
	{
	}

	bool DriveWipe::Run()
	{
		const File disk(m_path);

		if (!disk.IsValid())
		{
			m_errorCallback(TunkioStage::Open, 0, LastError);
			return false;
		}

		if (!disk.Size().first)
		{
			m_errorCallback(TunkioStage::Size, 0, LastError);
			return false;
		}
		
		// TODO: use FSCTL_DISMOUNT_VOLUME IOCTL

		const DataUnit::Mebibyte bufferSize(1);
		uint64_t bytesLeft = disk.Size().second;
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