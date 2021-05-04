#include "../KuuraAPI-PCH.hpp"
#include "KuuraDriveWorkload.hpp"
#include "../FillConsumers/KuuraDrive.hpp"
#include "../FillProviders/KuuraFillProvider.hpp"

namespace Kuura
{
	DriveWorkload::DriveWorkload(const CallbackContainer* callbacks, const std::filesystem::path& path) :
		IWorkload(callbacks, path, false)
	{
	}

	uint64_t DriveWorkload::Size()
	{
		if (!_drive)
		{
			_drive = std::make_shared<Drive>(Path);

			if (!_drive->IsValid())
			{
				_callbacks->OnError(Path.c_str(), KuuraStage::Open, 0, 0, LastError);
				return 0;
			}

			if (!_drive->Unmount())
			{
				_callbacks->OnError(Path.c_str(), KuuraStage::Unmount, 0, 0, LastError);
				return 0;
			}

			if (!_drive->Size())
			{
				_callbacks->OnError(Path.c_str(), KuuraStage::Size, 0, 0, LastError);
				return false;
			}
		}

		return _drive->Size().has_value() ? _drive->Size().value() : 0;
	}

	std::pair<bool, uint64_t> DriveWorkload::Run(const std::vector<std::shared_ptr<IFillProvider>>& fillers)
	{
		uint16_t passes = 0;
		uint64_t totalBytesWritten = 0;

		for (auto& filler : fillers)
		{
			_callbacks->OnPassStarted(Path.c_str(), ++passes);

			uint64_t bytesLeft = _drive->Size().value();
			uint64_t bytesWritten = 0;

			if (!Overwrite(passes, bytesLeft, bytesWritten, filler, _drive))
			{
				return { false, totalBytesWritten };
			}

			totalBytesWritten += bytesWritten;
			_callbacks->OnPassCompleted(Path.c_str(), passes);
		}

		return { true, totalBytesWritten };
	}
}
