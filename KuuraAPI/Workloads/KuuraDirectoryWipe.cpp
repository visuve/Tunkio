#include "../KuuraAPI-PCH.hpp"
#include "KuuraDirectoryWipe.hpp"
#include "../FillConsumers/KuuraDirectory.hpp"
#include "../FillProviders/KuuraFillProvider.hpp"

namespace Kuura
{
	DirectoryWipe::DirectoryWipe(
		const CallbackContainer& callbacks,
		const std::filesystem::path& path,
		bool removeAfterWipe) :
		IWorkload(callbacks, path, removeAfterWipe)
	{
	}

	bool DirectoryWipe::Run()
	{
		Directory directory(m_path);
		const auto path = m_path.string();

		std::optional<std::vector<std::shared_ptr<File>>>& files = directory.Files();

		if (!files)
		{
			m_callbacks.OnError(path.c_str(), KuuraStage::Open, 0, 0, LastError);
			return false;
		}

		std::optional<uint64_t> directorySize = directory.Size();

		if (!directorySize)
		{
			m_callbacks.OnError(path.c_str(), KuuraStage::Size, 0, 0, LastError);
			return false;
		}

		uint16_t passes = 0;
		uint64_t totalBytesWritten = 0;

		m_callbacks.OnWipeStarted(FillerCount(), directorySize.value());

		while (HasFillers())
		{
			m_callbacks.OnPassStarted(path.c_str(), ++passes);

			std::shared_ptr<IFillProvider> filler = TakeFiller();

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
					m_callbacks.OnError(path.c_str(), KuuraStage::Write, passes, bytesWritten, LastError);
				}

				totalBytesWritten += bytesWritten;
			}

			m_callbacks.OnPassCompleted(path.c_str(), passes);
		}

		if (m_removeAfterWipe && !directory.RemoveAll())
		{
			m_callbacks.OnError(path.c_str(), KuuraStage::Delete, passes, totalBytesWritten, LastError);
			return false;
		}

		m_callbacks.OnWipeCompleted(passes, totalBytesWritten);
		return true;
	}
}
