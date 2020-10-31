#include "PCH.hpp"
#include "Workloads/TunkioDriveWipe.hpp"

namespace Tunkio
{
	DriveWipe::DriveWipe(const std::string& path) :
		IWorkload(path)
	{
	}

	bool DriveWipe::Run()
	{
		m_startedCallback(1);
		m_progressCallback(1);
		m_completedCallback(1);
		return true;
	}
}