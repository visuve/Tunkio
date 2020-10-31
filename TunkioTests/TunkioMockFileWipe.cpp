#include "PCH.hpp"
#include "Workloads/TunkioFileWipe.hpp"

namespace Tunkio
{
	FileWipe::FileWipe(const std::string& path) :
		IWorkload(path)
	{
	}

	bool FileWipe::Run()
	{
		m_startedCallback(1);
		m_progressCallback(1);
		m_completedCallback(1);
		return true;
	}
}