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

	bool DriveWorkload::Run(const std::vector<std::shared_ptr<IFillProvider>>& fillers)
	{
		auto drive = std::make_shared<Drive>(_path);

		if (!drive->IsValid())
		{
			_callbacks->OnError(_path.c_str(), KuuraStage::Open, 0, 0, LastError);
			return false;
		}

		if (!drive->Unmount())
		{
			// TODO: maybe add a stage "unmount" which is only used for drives
			_callbacks->OnError(_path.c_str(), KuuraStage::Unmount, 0, 0, LastError);
			return false;
		}

		if (!drive->Size())
		{
			_callbacks->OnError(_path.c_str(), KuuraStage::Size, 0, 0, LastError);
			return false;
		}

		uint16_t passes = 0;
		uint64_t totalBytesWritten = 0;

		_callbacks->OnOverwriteStarted(static_cast<uint16_t>(fillers.size()), drive->Size().value());

		for (auto& filler : fillers)
		{
			_callbacks->OnPassStarted(_path.c_str(), ++passes);

			uint64_t bytesLeft = drive->Size().value();
			uint64_t bytesWritten = 0;

			if (!Overwrite(passes, bytesLeft, bytesWritten, filler, drive))
			{
				return false;
			}

			totalBytesWritten += bytesWritten;
			_callbacks->OnPassCompleted(_path.c_str(), passes);
		}

		_callbacks->OnOverwriteCompleted(passes, totalBytesWritten);

		return true;
	}
}
