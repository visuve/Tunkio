#include "TunkioTests-PCH.hpp"
#include "Workloads/TunkioDriveWipe.hpp"

namespace Tunkio
{
	DriveWipe::DriveWipe(const std::filesystem::path& path, void* context) :
		FileWipe(path, false, context)
	{
	}

	bool DriveWipe::Run()
	{
		return FileWipe::Run();
	}
}
