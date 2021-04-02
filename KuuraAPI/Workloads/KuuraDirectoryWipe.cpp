#include "../KuuraAPI-PCH.hpp"
#include "KuuraDirectoryWipe.hpp"
#include "../FillConsumers/KuuraDirectory.hpp"
#include "../FillProviders/KuuraFillProvider.hpp"

namespace Kuura
{
	DirectoryWipe::DirectoryWipe(
		const Composer* parent,
		const std::filesystem::path& path,
		bool removeAfterWipe) :
		IWorkload(parent, path, removeAfterWipe)
	{
	}

	bool DirectoryWipe::Run()
	{
		Directory directory(m_path);
		const auto path = m_path.string();

		std::optional<std::vector<std::shared_ptr<File>>>& files = directory.Files();

		if (!files)
		{
			m_parent->Callbacks.OnError(path.c_str(), KuuraStage::Open, 0, 0, LastError);
			return false;
		}

		std::optional<uint64_t> directorySize = directory.Size();

		if (!directorySize)
		{
			m_parent->Callbacks.OnError(path.c_str(), KuuraStage::Size, 0, 0, LastError);
			return false;
		}

		uint16_t passes = 0;
		uint64_t totalBytesWritten = 0;

		auto fillers  = m_parent->Fillers();
		m_parent->Callbacks.OnWipeStarted(static_cast<uint16_t>(fillers.size()), directorySize.value());

		for (auto& filler : fillers)
		{
			m_parent->Callbacks.OnPassStarted(path.c_str(), ++passes);

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
					m_parent->Callbacks.OnError(path.c_str(), KuuraStage::Write, passes, bytesWritten, LastError);
				}

				totalBytesWritten += bytesWritten;
			}

			m_parent->Callbacks.OnPassCompleted(path.c_str(), passes);
		}

		if (m_removeAfterWipe && !directory.RemoveAll())
		{
			m_parent->Callbacks.OnError(path.c_str(), KuuraStage::Delete, passes, totalBytesWritten, LastError);
			return false;
		}

		m_parent->Callbacks.OnWipeCompleted(passes, totalBytesWritten);
		return true;
	}
}
