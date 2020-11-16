#include "../TunkioAPI-PCH.hpp"
#include "TunkioFileWipe.hpp"
#include "../TunkioFile.hpp"
#include "../FillProviders/TunkioFillProvider.hpp"

namespace Tunkio
{
	FileWipe::FileWipe(void* context, const std::string& path) :
		IWorkload(context, path)
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

		uint16_t iteration = 0;
		uint64_t bytesLeft = file.Size().second;
		uint64_t bytesWritten = 0;

		OnStarted(static_cast<uint16_t>(m_fillers.size()), bytesLeft);

		while (!m_fillers.empty())
		{
			++iteration;

			std::shared_ptr<IFillProvider> filler = m_fillers.front();

			while (bytesLeft)
			{
				const auto result = file.Write(filler->Data(), filler->Size(bytesLeft));

				bytesWritten += result.second;
				bytesLeft -= result.second;

				if (!result.first)
				{
					OnError(TunkioStage::Write, iteration, bytesWritten, LastError);

					return false;
				}

				if (!OnProgress(iteration, bytesWritten))
				{
					return true;
				}
			}

			m_fillers.pop();
		}

		OnCompleted(iteration, bytesWritten);

		if (m_removeAfterFill && !file.Remove())
		{
			OnError(TunkioStage::Remove, iteration, bytesWritten, LastError);
			return false;
		}

		return true;
	}
}
