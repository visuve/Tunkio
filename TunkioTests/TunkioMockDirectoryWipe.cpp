#include "PCH.hpp"
#include "Workloads/TunkioDirectoryWipe.hpp"

namespace Tunkio
{
	DirectoryWipe::DirectoryWipe(const std::string& path) :
		IWorkload(path)
	{
	}

	bool DirectoryWipe::Run()
	{
		m_startedCallback(1, 1);
		m_progressCallback(1, 1);
		m_completedCallback(1, 1);
		return true;
	}
}