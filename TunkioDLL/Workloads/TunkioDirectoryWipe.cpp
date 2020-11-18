#include "../TunkioAPI-PCH.hpp"
#include "TunkioDirectoryWipe.hpp"
#include "../TunkioFile.hpp"
#include "../FillProviders/TunkioFillProvider.hpp"

namespace Tunkio
{
	DirectoryWipe::DirectoryWipe(void* context, const std::string& path) :
		IWorkload(context, path)
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

			const File file(entry.path().string());

			if (file.IsValid())
			{
				OnError(TunkioStage::Open, 0, 0, LastError);
				return false;
			}

			if (!file.Size().first)
			{
				OnError(TunkioStage::Size, 0, 0, LastError);
				return false;
			}

			files.emplace_back(file);
		}

		const auto sum = [](uint64_t sum, const File& file)
		{
			return sum + file.Size().second;
		};

		uint16_t iteration = 0;
		const uint64_t totalBytesLeft =
			std::accumulate(files.cbegin(), files.cend(), uint64_t(0), sum);

		uint64_t totalBytesWritten = 0;

		OnStarted(static_cast<uint16_t>(m_fillers.size()), totalBytesLeft);

		while (!m_fillers.empty())
		{
			OnIterationStarted(++iteration);

			std::shared_ptr<IFillProvider> filler = m_fillers.front();
			uint64_t bytesWritten = 0;

			for (File& file : files)
			{
				uint64_t bytesLeft = file.Size().second;

				while (bytesLeft)
				{
					const auto result = file.Write(filler->Data(), filler->Size(bytesLeft));

					bytesWritten += result.second;
					bytesLeft -= result.second;

					if (!result.first)
					{
						OnError(TunkioStage::Write, iteration, bytesWritten, LastError);
						return false;
					}

					if (!OnProgress(iteration, bytesWritten))
					{
						return true;
					}
				}
			}

			totalBytesWritten += bytesWritten;
			OnIterationCompleted(iteration);
		}

		if (m_removeAfterFill)
		{
			for (File& file : files)
			{
				if (!file.Remove())
				{
					OnError(TunkioStage::Remove, iteration, totalBytesWritten, LastError);
					return false;
				}
			}
		}

		OnCompleted(iteration, totalBytesWritten);
		return true;
	}
}
