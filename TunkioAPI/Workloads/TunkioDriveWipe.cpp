#include "../TunkioAPI-PCH.hpp"
#include "TunkioDriveWipe.hpp"
#include "../TunkioFile.hpp"
#include "../FillProviders/TunkioFillProvider.hpp"

namespace Tunkio
{
	DriveWipe::DriveWipe(const std::filesystem::path& path, void* context) :
		FileWipe(path, false, context)
	{
	}

	bool DriveWipe::Run()
	{
		File drive(m_path);

		if (!drive.IsValid())
		{
			OnError(TunkioStage::Open, 0, 0, LastError);
			return false;
		}

		if (!drive.Unmount())
		{
			// TODO: maybe add a stage "unmount" which is only used for drives
			OnError(TunkioStage::Unmount, 0, 0, LastError);
			return false;
		}

		if (!drive.ActualSize().first)
		{
			OnError(TunkioStage::Size, 0, 0, LastError);
			return false;
		}

		uint16_t passes = 0;
		uint64_t totalBytesWritten = 0;

		OnWipeStarted(FillerCount(), drive.ActualSize().second);

		while (HasFillers())
		{
			OnPassStarted(++passes);

			uint64_t bytesLeft = drive.ActualSize().second;
			uint64_t bytesWritten = 0;
			std::shared_ptr<IFillProvider> filler = TakeFiller();

			if (!Fill(passes, bytesLeft, bytesWritten, filler, drive))
			{
				return false;
			}

			if (!drive.Rewind())
			{
				OnError(TunkioStage::Rewind, passes, bytesWritten, LastError);
			}

			totalBytesWritten += bytesWritten;
			OnPassCompleted(passes);
		}

		OnWipeCompleted(passes, totalBytesWritten);

		return true;
	}
}
