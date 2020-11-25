#include "../TunkioAPI-PCH.hpp"
#include "TunkioFileWipe.hpp"
#include "../TunkioFile.hpp"
#include "../FillProviders/TunkioFillProvider.hpp"

namespace Tunkio
{
	FileWipe::FileWipe(
		const std::filesystem::path& path,
		bool removeAfterWipe,
		void* context) :
		IWorkload(path, removeAfterWipe, context)
	{
	}

	bool FileWipe::Run()
	{
		File file(m_path);

		if (!file.IsValid())
		{
			OnError(TunkioStage::Open, 0, 0, LastError);
			return false;
		}

		if (!file.Size().first)
		{
			OnError(TunkioStage::Size, 0, 0, LastError);
			return false;
		}

		uint16_t passes = 0;
		uint64_t totalBytesWritten = 0;

		OnWipeStarted(FillerCount(), file.Size().second);

		while (HasFillers())
		{
			OnPassStarted(++passes);

			uint64_t bytesWritten = 0;
			uint64_t bytesLeft = file.Size().second;

			std::shared_ptr<IFillProvider> filler = TakeFiller();

			while (bytesLeft)
			{
				const auto result = file.Write(filler->Data(), filler->Size(bytesLeft));

				bytesWritten += result.second;
				bytesLeft -= std::min(result.second, bytesLeft);

				if (!result.first)
				{
					OnError(TunkioStage::Write, passes, bytesWritten, LastError);
					return false;
				}

				if (!OnProgress(passes, bytesWritten))
				{
					return true;
				}
			}

			totalBytesWritten += bytesWritten;

			OnPassCompleted(passes);
		}

		OnWipeCompleted(passes, totalBytesWritten);

		if (m_removeAfterWipe && !file.Remove())
		{
			OnError(TunkioStage::Remove, passes, totalBytesWritten, LastError);
			return false;
		}

		return true;
	}
}
