#include "TunkioTests-PCH.hpp"
#include "Workloads/TunkioDriveWipe.hpp"

namespace Tunkio
{
	DriveWipe::DriveWipe(void* context, const std::filesystem::path& path) :
		FileWipe(context, path)
	{
	}

	bool DriveWipe::Run()
	{
		return FileWipe::Run();
	}
}
