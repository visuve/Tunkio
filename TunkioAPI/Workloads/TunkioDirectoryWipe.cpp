#include "../TunkioAPI-PCH.hpp"
#include "TunkioDirectoryWipe.hpp"
#include "../TunkioDirectory.hpp"
#include "../FillProviders/TunkioFillProvider.hpp"

namespace Tunkio
{
	DirectoryWipe::DirectoryWipe(
		const std::filesystem::path& path,
		bool removeAfterWipe,
		void* context) :
		IWorkload(path, removeAfterWipe, context)
	{
	}

	bool DirectoryWipe::Run()
	{
		Directory directory(m_path);

		std::pair<bool, std::vector<File>>& files = directory.Files();

		if (!files.first)
		{
			OnError(TunkioStage::Open, 0, 0, LastError);
			return false;
		}

		std::pair<bool, uint64_t> directorySize = directory.Size();

		if (!directorySize.first)
		{
			OnError(TunkioStage::Size, 0, 0, LastError);
			return false;
		}

		uint16_t passes = 0;
		uint64_t totalBytesWritten = 0;

		OnWipeStarted(FillerCount(), directorySize.second);

		while (HasFillers())
		{
			OnPassStarted(++passes);

			std::shared_ptr<IFillProvider> filler = TakeFiller();
			uint64_t bytesWritten = 0;

			for (File& file : files.second)
			{
				uint64_t bytesLeft = file.Size().second;

				while (bytesLeft)
				{
					const auto result = file.Write(filler->Data(), filler->Size(bytesLeft));

					bytesWritten += result.second;
					bytesLeft -= std::min(result.second, bytesLeft);

					if (!result.first)
					{
						OnError(TunkioStage::Write, passes, bytesWritten, LastError);
						return false;
					}

					if (!OnProgress(passes, bytesWritten))
					{
						return true;
					}
				}
			}

			totalBytesWritten += bytesWritten;
			OnPassCompleted(passes);
		}

		if (m_removeAfterWipe && !directory.RemoveAll())
		{
			OnError(TunkioStage::Remove, passes, totalBytesWritten, LastError);
			return false;
		}

		OnWipeCompleted(passes, totalBytesWritten);
		return true;
	}
}
