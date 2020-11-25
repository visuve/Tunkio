#include "../TunkioAPI-PCH.hpp"
#include "TunkioDriveWipe.hpp"

namespace Tunkio
{
	DriveWipe::DriveWipe(const std::filesystem::path& path, void* context) :
		FileWipe(path, false, context)
	{
	}

	bool DriveWipe::Run()
	{
		// TODO: use FSCTL_DISMOUNT_VOLUME IOCTL
		return FileWipe::Run();
	}
}
