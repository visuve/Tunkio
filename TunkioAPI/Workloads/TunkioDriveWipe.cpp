#include "../TunkioAPI-PCH.hpp"
#include "TunkioDriveWipe.hpp"

namespace Tunkio
{
	DriveWipe::DriveWipe(void* context, const std::string& path) :
		FileWipe(context, path)
	{
	}

	bool DriveWipe::Run()
	{
		// TODO: use FSCTL_DISMOUNT_VOLUME IOCTL
		return FileWipe::Run();
	}
}
