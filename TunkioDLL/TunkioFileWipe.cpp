#include "PCH.hpp"
#include "TunkioFileWipe.hpp"
#include "TunkioFillGenerator.hpp"
#include "TunkioFile.hpp"

namespace Tunkio
{
	FileWipe::FileWipe(const std::string& path) :
		IOperation(path)
	{
	}

	bool FileWipe::Run()
	{
		File file(m_path);

		if (file.IsValid())
		{
			m_errorCallback(TunkioStage::Open, 0, LastError);
			return false;
		}

		if (!file.Size())
		{
			m_errorCallback(TunkioStage::Size, 0, LastError);
		}

		const DataUnit::Mebibyte bufferSize(1);
		uint64_t bytesLeft = file.Size();
		uint64_t bytesWritten = 0;

		FillGenerator fakeData(m_fillMode, DataUnit::Mebibyte(1));

		m_startedCallback(bytesLeft);

		while (bytesLeft)
		{
			const uint64_t bytesToWrite = bufferSize > bytesLeft ? bytesLeft : bufferSize.Bytes();
			const auto result = file.Write(fakeData.Data(), bytesToWrite);

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

		if (m_removeAfterFill && !file.Remove())
		{
			m_errorCallback(TunkioStage::Remove, bytesWritten, LastError);
			return false;
		}

		return true;
	}
}