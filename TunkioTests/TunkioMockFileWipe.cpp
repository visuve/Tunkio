#include "TunkioTests-PCH.hpp"
#include "Workloads/TunkioFileWipe.hpp"

namespace Tunkio
{
	FileWipe::FileWipe(void* context, const std::filesystem::path& path) :
		IWorkload(context, path)
	{
	}

	bool FileWipe::Run()
	{
		OnWipeStarted(1, 1);

		while (!m_fillers.empty())
		{
			m_fillers.pop();
			OnPassStarted(1);
			OnProgress(1, 1);
			OnPassCompleted(1);
		}

		OnCompleted(1, 1);
		return true;
	}
}
