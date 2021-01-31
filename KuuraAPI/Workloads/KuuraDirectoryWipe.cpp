#include "../KuuraAPI-PCH.hpp"
#include "KuuraDirectoryWipe.hpp"
#include "../FillConsumers/KuuraDirectory.hpp"
#include "../FillProviders/KuuraFillProvider.hpp"

namespace Kuura
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

		std::pair<bool, std::vector<std::shared_ptr<File>>>& files = directory.Files();

		if (!files.first)
		{
			OnError(KuuraStage::Open, 0, 0, LastError);
			return false;
		}

		std::pair<bool, uint64_t> directorySize = directory.Size();

		if (!directorySize.first)
		{
			OnError(KuuraStage::Size, 0, 0, LastError);
			return false;
		}

		uint16_t passes = 0;
		uint64_t totalBytesWritten = 0;

		OnWipeStarted(FillerCount(), directorySize.second);

		while (HasFillers())
		{
			OnPassStarted(++passes);

			std::shared_ptr<IFillProvider> filler = TakeFiller();

			for (auto& file : files.second)
			{
				uint64_t bytesLeft = file->Size().value();
				uint64_t bytesWritten = 0;

				if (!Fill(passes, bytesLeft, bytesWritten, filler, file))
				{
					return false;
				}

				if (!file->Flush())
				{
					OnError(KuuraStage::Write, passes, bytesWritten, LastError);
				}

				totalBytesWritten += bytesWritten;
			}

			OnPassCompleted(passes);
		}

		if (m_removeAfterWipe && !directory.RemoveAll())
		{
			OnError(KuuraStage::Delete, passes, totalBytesWritten, LastError);
			return false;
		}

		OnWipeCompleted(passes, totalBytesWritten);
		return true;
	}
}