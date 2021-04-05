#include "../KuuraAPI-PCH.hpp"
#include "KuuraDirectoryWorkload.hpp"
#include "../FillConsumers/KuuraDirectory.hpp"
#include "../FillProviders/KuuraFillProvider.hpp"

namespace Kuura
{
	DirectoryWorkload::DirectoryWorkload(
		const CallbackContainer* callbacks,
		const std::filesystem::path& path,
		bool removeAfterOverwrite) :
		IWorkload(callbacks, path, removeAfterOverwrite)
	{
	}

	bool DirectoryWorkload::Run(const std::vector<std::shared_ptr<IFillProvider>>& fillers)
	{
		Directory directory(_path);

		std::optional<std::vector<std::shared_ptr<File>>>& files = directory.Files();

		if (!files)
		{
			_callbacks->OnError(_path.c_str(), KuuraStage::Open, 0, 0, LastError);
			return false;
		}

		std::optional<uint64_t> directorySize = directory.Size();

		if (!directorySize)
		{
			_callbacks->OnError(_path.c_str(), KuuraStage::Size, 0, 0, LastError);
			return false;
		}

		uint16_t passes = 0;
		uint64_t totalBytesWritten = 0;

		_callbacks->OnOverwriteStarted(static_cast<uint16_t>(fillers.size()), directorySize.value());

		for (auto& filler : fillers)
		{
			_callbacks->OnPassStarted(_path.c_str(), ++passes);

			for (auto& file : files.value())
			{
				uint64_t bytesLeft = file->Size().value();
				uint64_t bytesWritten = 0;

				if (!Overwrite(passes, bytesLeft, bytesWritten, filler, file))
				{
					return false;
				}

				if (!file->Flush())
				{
					_callbacks->OnError(_path.c_str(), KuuraStage::Write, passes, bytesWritten, LastError);
				}

				totalBytesWritten += bytesWritten;
			}

			_callbacks->OnPassCompleted(_path.c_str(), passes);
		}

		if (_removeAfterOverwrite && !directory.RemoveAll())
		{
			_callbacks->OnError(_path.c_str(), KuuraStage::Delete, passes, totalBytesWritten, LastError);
			return false;
		}

		_callbacks->OnOverwriteCompleted(passes, totalBytesWritten);
		return true;
	}
}
