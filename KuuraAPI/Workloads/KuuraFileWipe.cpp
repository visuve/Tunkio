#include "../KuuraAPI-PCH.hpp"
#include "KuuraFileWipe.hpp"
#include "../FillConsumers/KuuraFile.hpp"
#include "../FillProviders/KuuraFillProvider.hpp"

namespace Kuura
{
	FileWipe::FileWipe(
		const CallbackContainer& callbacks,
		const std::filesystem::path& path,
		bool removeAfterWipe) :
		IWorkload(callbacks, path, removeAfterWipe)
	{
	}

	bool FileWipe::Run()
	{
		auto file = std::make_shared<File>(m_path);
		const auto path = m_path.string();

		if (!file->IsValid())
		{
			m_callbacks.OnError(path.c_str(), KuuraStage::Open, 0, 0, LastError);
			return false;
		}

		if (!file->Size())
		{
			m_callbacks.OnError(path.c_str(), KuuraStage::Size, 0, 0, LastError);
			return false;
		}

		uint16_t passes = 0;
		uint64_t totalBytesWritten = 0;

		m_callbacks.OnWipeStarted(FillerCount(), file->Size().value());

		while (HasFillers())
		{
			m_callbacks.OnPassStarted(path.c_str(), ++passes);

			uint64_t bytesLeft = file->Size().value();
			uint64_t bytesWritten = 0;
			std::shared_ptr<IFillProvider> filler = TakeFiller();

			if (!Fill(passes, bytesLeft, bytesWritten, filler, file))
			{
				return false;
			}

			if (!file->Flush())
			{
				m_callbacks.OnError(path.c_str(), KuuraStage::Write, passes, bytesWritten, LastError);
			}

			totalBytesWritten += bytesWritten;
			m_callbacks.OnPassCompleted(path.c_str(), passes);
		}

		m_callbacks.OnWipeCompleted(passes, totalBytesWritten);

		if (m_removeAfterWipe && !file->Delete())
		{
			m_callbacks.OnError(path.c_str(), KuuraStage::Delete, passes, totalBytesWritten, LastError);
			return false;
		}

		return true;
	}
}
