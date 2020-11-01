#include "PCH.hpp"
#include "Workloads/TunkioDriveWipe.hpp"

namespace Tunkio
{
	DriveWipe::DriveWipe(const std::string& path) :
		FileWipe(path)
	{
	}

	bool DriveWipe::Run()
	{
		return FileWipe::Run();
	}
}