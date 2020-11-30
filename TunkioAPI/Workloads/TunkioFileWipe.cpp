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

		if (!file.AllocationSize().first)
		{
			OnError(TunkioStage::Size, 0, 0, LastError);
			return false;
		}

		uint16_t passes = 0;
		uint64_t totalBytesWritten = 0;

		OnWipeStarted(FillerCount(), file.AllocationSize().second);

		while (HasFillers())
		{
			OnPassStarted(++passes);

			uint64_t bytesLeft = file.AllocationSize().second;
			uint64_t bytesWritten = 0;
			std::shared_ptr<IFillProvider> filler = TakeFiller();

			if (!Fill(passes, bytesLeft, bytesWritten, filler, file))
			{
				return false;
			}

			if (!file.Flush())
			{
				OnError(TunkioStage::Write, passes, bytesWritten, LastError);
			}

			if (!file.Rewind())
			{
				OnError(TunkioStage::Rewind, passes, bytesWritten, LastError);
			}

			totalBytesWritten += bytesWritten;
			OnPassCompleted(passes);
		}

		OnWipeCompleted(passes, totalBytesWritten);

		if (m_removeAfterWipe && !file.Delete())
		{
			OnError(TunkioStage::Delete, passes, totalBytesWritten, LastError);
			return false;
		}

		return true;
	}
}
