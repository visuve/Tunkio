#include "../TunkioAPI-PCH.hpp"
#include "TunkioFileWipe.hpp"
#include "../TunkioFile.hpp"
#include "../FillProviders/TunkioFillProvider.hpp"

namespace Tunkio
{
	FileWipe::FileWipe(void* context, const std::filesystem::path& path) :
		IWorkload(context, path)
	{
	}

	bool FileWipe::Run()
	{
		File file(m_path);

		if (!file.IsValid())
		{
			OnWipeError(TunkioStage::Open, 0, 0, LastError);
			return false;
		}

		if (!file.Size().first)
		{
			OnWipeError(TunkioStage::Size, 0, 0, LastError);
			return false;
		}

		uint16_t passes = 0;
		uint64_t totalBytesWritten = 0;

		OnWipeStarted(static_cast<uint16_t>(m_fillers.size()), file.Size().second);

		while (!m_fillers.empty())
		{
			OnPassStarted(++passes);

			uint64_t bytesWritten = 0;
			uint64_t bytesLeft = file.Size().second;

			std::shared_ptr<IFillProvider> filler = m_fillers.front();

			while (bytesLeft)
			{
				const auto result = file.Write(filler->Data(), filler->Size(bytesLeft));

				bytesWritten += result.second;
				bytesLeft -= std::min(result.second, bytesLeft);

				if (!result.first)
				{
					OnWipeError(TunkioStage::Write, passes, bytesWritten, LastError);
					return false;
				}

				if (!OnProgress(passes, bytesWritten))
				{
					return true;
				}
			}

			totalBytesWritten += bytesWritten;
			m_fillers.pop();

			OnPassCompleted(passes);
		}

		OnCompleted(passes, totalBytesWritten);

		if (m_removeAfterFill && !file.Remove())
		{
			OnWipeError(TunkioStage::Remove, passes, totalBytesWritten, LastError);
			return false;
		}

		return true;
	}
}
