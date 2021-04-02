#include "../KuuraAPI-PCH.hpp"
#include "KuuraFileWipe.hpp"
#include "../FillConsumers/KuuraFile.hpp"
#include "../FillProviders/KuuraFillProvider.hpp"

namespace Kuura
{
	FileWipe::FileWipe(
		const Composer* parent,
		const std::filesystem::path& path,
		bool removeAfterWipe) :
		IWorkload(parent, path, removeAfterWipe)
	{
	}

	bool FileWipe::Run()
	{
		auto file = std::make_shared<File>(m_path);
		const auto path = m_path.string();

		if (!file->IsValid())
		{
			m_parent->Callbacks.OnError(path.c_str(), KuuraStage::Open, 0, 0, LastError);
			return false;
		}

		if (!file->Size())
		{
			m_parent->Callbacks.OnError(path.c_str(), KuuraStage::Size, 0, 0, LastError);
			return false;
		}

		uint16_t passes = 0;
		uint64_t totalBytesWritten = 0;

		auto fillers = m_parent->Fillers();

		m_parent->Callbacks.OnWipeStarted(static_cast<uint16_t>(fillers.size()), file->Size().value());

		for (auto filler : fillers)
		{
			m_parent->Callbacks.OnPassStarted(path.c_str(), ++passes);

			uint64_t bytesLeft = file->Size().value();
			uint64_t bytesWritten = 0;

			if (!Fill(passes, bytesLeft, bytesWritten, filler, file))
			{
				return false;
			}

			if (!file->Flush())
			{
				m_parent->Callbacks.OnError(path.c_str(), KuuraStage::Write, passes, bytesWritten, LastError);
			}

			totalBytesWritten += bytesWritten;
			m_parent->Callbacks.OnPassCompleted(path.c_str(), passes);
		}

		m_parent->Callbacks.OnWipeCompleted(passes, totalBytesWritten);

		if (m_removeAfterWipe && !file->Delete())
		{
			m_parent->Callbacks.OnError(path.c_str(), KuuraStage::Delete, passes, totalBytesWritten, LastError);
			return false;
		}

		return true;
	}
}
