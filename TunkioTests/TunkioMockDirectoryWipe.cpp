#include "TunkioTests-PCH.hpp"
#include "Workloads/TunkioDirectoryWipe.hpp"

namespace Tunkio
{
	DirectoryWipe::DirectoryWipe(void* context, const std::string& path) :
		IWorkload(context, path)
	{
	}

	bool DirectoryWipe::Run()
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
