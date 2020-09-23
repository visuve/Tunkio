#include "PCH.hpp"
#include "TunkioDirectoryWipe.hpp"
#include "TunkioFillGenerator.hpp"
#include "TunkioFile.hpp"

namespace Tunkio
{
	DirectoryWipe::DirectoryWipe(const TunkioOptions* options) :
		IOperation(options)
	{
	}

	bool DirectoryWipe::Run()
	{
		const std::string path(m_options->Path.Data, m_options->Path.Length);

		std::vector<File> files;

		for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
		{
			if (!entry.is_regular_file())
			{
				continue;
			}

			const File file(entry.path());

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
		FillGenerator fakeData(m_options->Mode, bufferSize);

		m_options->Callbacks.StartedCallback(totalBytesLeft);

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
					m_options->Callbacks.ErrorCallback(
						TunkioStage::Write,
						totalBytesWritten,
						LastError);

					return false;
				}

				if (!m_options->Callbacks.ProgressCallback(totalBytesWritten))
				{
					return true;
				}
			}

			if (m_options->Remove && !file.Delete())
			{
				m_options->Callbacks.ErrorCallback(
					TunkioStage::Delete,
					totalBytesWritten,
					LastError);

				return false;
			}
		}

		m_options->Callbacks.CompletedCallback(totalBytesWritten);
		return true;
	}
}