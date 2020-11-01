#include "../PCH.hpp"
#include "TunkioDriveWipe.hpp"

namespace Tunkio
{
	DriveWipe::DriveWipe(const std::string& path) :
		FileWipe(path)
	{
	}

	bool DriveWipe::Run()
	{
		// TODO: use FSCTL_DISMOUNT_VOLUME IOCTL
		return FileWipe::Run();
	}
}