#include "../KuuraAPI-PCH.hpp"
#include "KuuraDirectoryOverwrite.hpp"
#include "../FillConsumers/KuuraDirectory.hpp"
#include "../FillProviders/KuuraFillProvider.hpp"

namespace Kuura
{
	DirectoryOverwrite::DirectoryOverwrite(
		const Composer* parent,
		const std::filesystem::path& path,
		bool removeAfterOverwrite) :
		IWorkload(parent, path, removeAfterOverwrite)
	{
	}

	bool DirectoryOverwrite::Run()
	{
		Directory directory(m_path);

		std::optional<std::vector<std::shared_ptr<File>>>& files = directory.Files();

		if (!files)
		{
			m_parent->Callbacks.OnError(m_path.c_str(), KuuraStage::Open, 0, 0, LastError);
			return false;
		}

		std::optional<uint64_t> directorySize = directory.Size();

		if (!directorySize)
		{
			m_parent->Callbacks.OnError(m_path.c_str(), KuuraStage::Size, 0, 0, LastError);
			return false;
		}

		uint16_t passes = 0;
		uint64_t totalBytesWritten = 0;

		auto fillers  = m_parent->Fillers();
		m_parent->Callbacks.OnOverwriteStarted(static_cast<uint16_t>(fillers.size()), directorySize.value());

		for (auto& filler : fillers)
		{
			m_parent->Callbacks.OnPassStarted(m_path.c_str(), ++passes);

			for (auto& file : files.value())
			{
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
			}

			m_parent->Callbacks.OnPassCompleted(m_path.c_str(), passes);
		}

		if (m_removeAfterOverwrite && !directory.RemoveAll())
		{
			m_parent->Callbacks.OnError(m_path.c_str(), KuuraStage::Delete, passes, totalBytesWritten, LastError);
			return false;
		}

		m_parent->Callbacks.OnOverwriteCompleted(passes, totalBytesWritten);
		return true;
	}
}