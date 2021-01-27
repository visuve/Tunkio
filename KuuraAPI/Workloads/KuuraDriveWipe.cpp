#include "../KuuraAPI-PCH.hpp"
#include "KuuraDriveWipe.hpp"
#include "../FillConsumers/KuuraDrive.hpp"
#include "../FillProviders/KuuraFillProvider.hpp"

namespace Kuura
{
	DriveWipe::DriveWipe(const std::filesystem::path& path, void* context) :
		FileWipe(path, false, context)
	{
	}

	bool DriveWipe::Run()
	{
		auto drive = std::make_shared<Drive>(m_path);

		if (!drive->IsValid())
		{
			OnError(KuuraStage::Open, 0, 0, LastError);
			return false;
		}

		if (!drive->Unmount())
		{
			// TODO: maybe add a stage "unmount" which is only used for drives
			OnError(KuuraStage::Unmount, 0, 0, LastError);
			return false;
		}

		if (!drive->Size().first)
		{
			OnError(KuuraStage::Size, 0, 0, LastError);
			return false;
		}

		uint16_t passes = 0;
		uint64_t totalBytesWritten = 0;

		OnWipeStarted(FillerCount(), drive->Size().second);

		while (HasFillers())
		{
			OnPassStarted(++passes);

			uint64_t bytesLeft = drive->Size().second;
			uint64_t bytesWritten = 0;
			std::shared_ptr<IFillProvider> filler = TakeFiller();

			if (!Fill(passes, bytesLeft, bytesWritten, filler, drive))
			{
				return false;
			}

			totalBytesWritten += bytesWritten;
			OnPassCompleted(passes);
		}

		OnWipeCompleted(passes, totalBytesWritten);

		return true;
	}
}
