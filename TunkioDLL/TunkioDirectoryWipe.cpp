#include "PCH.hpp"
#include "TunkioDirectoryWipe.hpp"
#include "TunkioFillGenerator.hpp"
#include "TunkioFile.hpp"

namespace Tunkio
{
	DirectoryWipe::DirectoryWipe(const std::string& path) :
		IOperation(path)
	{
	}

	bool DirectoryWipe::Run()
	{
		std::vector<File> files;

		for (const auto& entry : std::filesystem::recursive_directory_iterator(m_path))
		{
			if (!entry.is_regular_file())
			{
				continue;
			}

			const File file(entry.path());

			if (file.IsValid())
			{
				m_errorCallback(
					TunkioStage::Open,
					0,
					LastError);

				return false;
			}

			if (!file.Size())
			{
				m_errorCallback(
					TunkioStage::Size,
					0,
					LastError);
			}

			files.emplace_back(file);
		}

		const auto sum = [](uint64_t sum, const File& file)
		{
			return sum + file.Size();
		};

		const uint64_t totalBytesLeft =
			std::accumulate(files.cbegin(), files.cend(), uint64_t(0), sum);

		uint64_t totalBytesWritten = 0;

		const DataUnit::Mebibyte bufferSize(1);
		FillGenerator fakeData(m_fillMode, bufferSize);

		m_startedCallback(totalBytesLeft);

		for (File& file : files)
		{
			uint64_t bytesLeft = file.Size();

			while (bytesLeft)
			{
				const uint64_t bytesToWrite = bufferSize > bytesLeft ? bytesLeft : bufferSize.Bytes();
				const auto result = file.Write(fakeData.Data(), bytesToWrite);

				totalBytesWritten += result.second;
				bytesLeft -= result.second;

				if (!result.first)
				{
					m_errorCallback(
						TunkioStage::Write,
						totalBytesWritten,
						LastError);

					return false;
				}

				if (!m_progressCallback(totalBytesWritten))
				{
					return true;
				}
			}

			if (m_removeAfterFill && !file.Delete())
			{
				m_errorCallback(
					TunkioStage::Delete,
					totalBytesWritten,
					LastError);

				return false;
			}
		}

		m_completedCallback(totalBytesWritten);
		return true;
	}
}