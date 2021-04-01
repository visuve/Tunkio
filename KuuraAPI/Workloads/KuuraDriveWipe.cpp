#include "../KuuraAPI-PCH.hpp"
#include "KuuraDriveWipe.hpp"
#include "../FillConsumers/KuuraDrive.hpp"
#include "../FillProviders/KuuraFillProvider.hpp"

namespace Kuura
{
	DriveWipe::DriveWipe(const CallbackContainer& callbacks, const std::filesystem::path& path) :
		FileWipe(callbacks, path, false)
	{
	}

	bool DriveWipe::Run()
	{
		auto drive = std::make_shared<Drive>(m_path);
		const auto path = m_path.string();

		if (!drive->IsValid())
		{
			m_callbacks.OnError(path.c_str(), KuuraStage::Open, 0, 0, LastError);
			return false;
		}

		if (!drive->Unmount())
		{
			// TODO: maybe add a stage "unmount" which is only used for drives
			m_callbacks.OnError(path.c_str(), KuuraStage::Unmount, 0, 0, LastError);
			return false;
		}

		if (!drive->Size())
		{
			m_callbacks.OnError(path.c_str(), KuuraStage::Size, 0, 0, LastError);
			return false;
		}

		uint16_t passes = 0;
		uint64_t totalBytesWritten = 0;

		m_callbacks.OnWipeStarted(FillerCount(), drive->Size().value());

		while (HasFillers())
		{
			m_callbacks.OnPassStarted(path.c_str(), ++passes);

			uint64_t bytesLeft = drive->Size().value();
			uint64_t bytesWritten = 0;
			std::shared_ptr<IFillProvider> filler = TakeFiller();

			if (!Fill(passes, bytesLeft, bytesWritten, filler, drive))
			{
				return false;
			}

			totalBytesWritten += bytesWritten;
			m_callbacks.OnPassCompleted(path.c_str(), passes);
		}

		m_callbacks.OnWipeCompleted(passes, totalBytesWritten);

		return true;
	}
}
