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

	uint64_t DirectoryWorkload::Size()
	{
		if (!_directory)
		{
			_directory = std::make_shared<Directory>(Path);

			std::optional<std::vector<std::shared_ptr<File>>>& files = _directory->Files();

			if (!files)
			{
				_callbacks->OnError(Path.c_str(), KuuraStage::Open, 0, 0, LastError);
				return 0;
			}

			if (!_directory->Size())
			{
				_callbacks->OnError(Path.c_str(), KuuraStage::Size, 0, 0, LastError);
				return 0;
			}
		}

		return _directory->Size().has_value() ? _directory->Size().value() : 0;
	}

	std::pair<bool, uint64_t> DirectoryWorkload::Run(const std::vector<std::shared_ptr<IFillProvider>>& fillers)
	{
		uint16_t passes = 0;
		uint64_t totalBytesWritten = 0;

		for (auto& filler : fillers)
		{
			_callbacks->OnPassStarted(Path.c_str(), ++passes);

			for (auto& file : _directory->Files().value())
			{
				uint64_t bytesLeft = file->Size().value();
				uint64_t bytesWritten = 0;

				if (!Overwrite(passes, bytesLeft, bytesWritten, filler, file))
				{
					return { false, totalBytesWritten };
				}

				if (!file->Flush())
				{
					_callbacks->OnError(Path.c_str(), KuuraStage::Write, passes, bytesWritten, LastError);
					return { false, totalBytesWritten };
				}

				totalBytesWritten += bytesWritten;
			}

			_callbacks->OnPassCompleted(Path.c_str(), passes);
		}

		if (_removeAfterOverwrite && !_directory->RemoveAll())
		{
			_callbacks->OnError(Path.c_str(), KuuraStage::Delete, passes, totalBytesWritten, LastError);
			return { false, totalBytesWritten };
		}

		return { true, totalBytesWritten };
	}
}
