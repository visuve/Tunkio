#include "../KuuraAPI-PCH.hpp"
#include "KuuraFileWorkload.hpp"
#include "../FillConsumers/KuuraFile.hpp"
#include "../FillProviders/KuuraFillProvider.hpp"

namespace Kuura
{
	FileWorkload::FileWorkload(
		const CallbackContainer* callbacks,
		const std::filesystem::path& path,
		bool removeAfterOverwrite) :
		IWorkload(callbacks, path, removeAfterOverwrite)
	{
	}

	uint64_t FileWorkload::Size()
	{
		if (!_file)
		{
			_file = std::make_shared<File>(Path);

			if (!_file->IsValid())
			{
				_callbacks->OnError(Path.c_str(), KuuraStage::Open, 0, 0, LastError);
				return 0;
			}

			if (!_file->Size())
			{
				_callbacks->OnError(Path.c_str(), KuuraStage::Size, 0, 0, LastError);
				return 0;
			}
		}

		return _file->Size().has_value() ? _file->Size().value() : 0;
	}

	std::pair<bool, uint64_t> FileWorkload::Run(const std::vector<std::shared_ptr<IFillProvider>>& fillers)
	{
		uint16_t passes = 0;
		uint64_t totalBytesWritten = 0;

		for (auto filler : fillers)
		{
			_callbacks->OnPassStarted(Path.c_str(), ++passes);

			uint64_t bytesLeft = _file->Size().value();
			uint64_t bytesWritten = 0;

			if (!Overwrite(passes, bytesLeft, bytesWritten, filler, _file))
			{
				return { false, totalBytesWritten };
			}

			if (!_file->Flush())
			{
				_callbacks->OnError(Path.c_str(), KuuraStage::Write, passes, bytesWritten, LastError);
				return { false, totalBytesWritten };
			}

			totalBytesWritten += bytesWritten;
			_callbacks->OnPassCompleted(Path.c_str(), passes);
		}

		if (_removeAfterOverwrite && !_file->Delete())
		{
			_callbacks->OnError(Path.c_str(), KuuraStage::Delete, passes, totalBytesWritten, LastError);
			return { false, totalBytesWritten };
		}

		return { true, totalBytesWritten };
	}
}
