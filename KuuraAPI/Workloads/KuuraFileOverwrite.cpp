#include "../KuuraAPI-PCH.hpp"
#include "KuuraFileOverwrite.hpp"
#include "../FillConsumers/KuuraFile.hpp"
#include "../FillProviders/KuuraFillProvider.hpp"

namespace Kuura
{
	FileOverwrite::FileOverwrite(
		const Composer* parent,
		const std::filesystem::path& path,
		bool removeAfterOverwrite) :
		IWorkload(parent, path, removeAfterOverwrite)
	{
	}

	bool FileOverwrite::Run()
	{
		auto file = std::make_shared<File>(m_path);

		if (!file->IsValid())
		{
			m_parent->Callbacks.OnError(m_path.c_str(), KuuraStage::Open, 0, 0, LastError);
			return false;
		}

		if (!file->Size())
		{
			m_parent->Callbacks.OnError(m_path.c_str(), KuuraStage::Size, 0, 0, LastError);
			return false;
		}

		uint16_t passes = 0;
		uint64_t totalBytesWritten = 0;

		auto fillers = m_parent->Fillers();

		m_parent->Callbacks.OnOverwriteStarted(static_cast<uint16_t>(fillers.size()), file->Size().value());

		for (auto filler : fillers)
		{
			m_parent->Callbacks.OnPassStarted(m_path.c_str(), ++passes);

			uint64_t bytesLeft = file->Size().value();
			uint64_t bytesWritten = 0;

			if (!Fill(passes, bytesLeft, bytesWritten, filler, file))
			{
				return false;
			}

			if (!file->Flush())
			{
				m_parent->Callbacks.OnError(m_path.c_str(), KuuraStage::Write, passes, bytesWritten, LastError);
			}

			totalBytesWritten += bytesWritten;
			m_parent->Callbacks.OnPassCompleted(m_path.c_str(), passes);
		}

		m_parent->Callbacks.OnOverwriteCompleted(passes, totalBytesWritten);

		if (m_removeAfterOverwrite && !file->Delete())
		{
			m_parent->Callbacks.OnError(m_path.c_str(), KuuraStage::Delete, passes, totalBytesWritten, LastError);
			return false;
		}

		return true;
	}
}
