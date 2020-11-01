#include "../PCH.hpp"
#include "TunkioFileWipe.hpp"
#include "../TunkioFile.hpp"
#include "../FillProviders/TunkioFillProvider.hpp"

namespace Tunkio
{
	FileWipe::FileWipe(const std::string& path) :
		IWorkload(path)
	{
	}

	bool FileWipe::Run()
	{
		File file(m_path);

		if (!file.IsValid())
		{
			m_errorCallback(TunkioStage::Open, 0, 0, LastError);
			return false;
		}

		if (!file.Size().first)
		{
			m_errorCallback(TunkioStage::Size, 0, 0, LastError);
			return false;
		}

		uint16_t iteration = 0;
		uint64_t bytesLeft = file.Size().second;
		uint64_t bytesWritten = 0;

		m_startedCallback(static_cast<uint16_t>(m_fillers.size()), bytesLeft);

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
					m_errorCallback(TunkioStage::Write, iteration, bytesWritten, LastError);
					return false;
				}

				if (!m_progressCallback(iteration, bytesWritten))
				{
					return true;
				}
			}

			m_fillers.pop();
		}

		m_completedCallback(iteration, bytesWritten);

		if (m_removeAfterFill && !file.Remove())
		{
			m_errorCallback(TunkioStage::Remove, iteration, bytesWritten, LastError);
			return false;
		}

		return true;
	}
}