#include "../KuuraAPI-PCH.hpp"
#include "KuuraDriveOverwrite.hpp"
#include "../FillConsumers/KuuraDrive.hpp"
#include "../FillProviders/KuuraFillProvider.hpp"

namespace Kuura
{
	DriveOverwrite::DriveOverwrite(const Composer* parent, const std::filesystem::path& path) :
		FileOverwrite(parent, path, false)
	{
	}

	bool DriveOverwrite::Run()
	{
		auto drive = std::make_shared<Drive>(m_path);

		if (!drive->IsValid())
		{
			m_parent->Callbacks.OnError(m_path.c_str(), KuuraStage::Open, 0, 0, LastError);
			return false;
		}

		if (!drive->Unmount())
		{
			// TODO: maybe add a stage "unmount" which is only used for drives
			m_parent->Callbacks.OnError(m_path.c_str(), KuuraStage::Unmount, 0, 0, LastError);
			return false;
		}

		if (!drive->Size())
		{
			m_parent->Callbacks.OnError(m_path.c_str(), KuuraStage::Size, 0, 0, LastError);
			return false;
		}

		uint16_t passes = 0;
		uint64_t totalBytesWritten = 0;

		auto fillers = m_parent->Fillers();
		m_parent->Callbacks.OnOverwriteStarted(static_cast<uint16_t>(fillers.size()), drive->Size().value());

		for (auto& filler : fillers)
		{
			m_parent->Callbacks.OnPassStarted(m_path.c_str(), ++passes);

			uint64_t bytesLeft = drive->Size().value();
			uint64_t bytesWritten = 0;

			if (!Fill(passes, bytesLeft, bytesWritten, filler, drive))
			{
				return false;
			}

			totalBytesWritten += bytesWritten;
			m_parent->Callbacks.OnPassCompleted(m_path.c_str(), passes);
		}

		m_parent->Callbacks.OnOverwriteCompleted(passes, totalBytesWritten);

		return true;
	}
}
