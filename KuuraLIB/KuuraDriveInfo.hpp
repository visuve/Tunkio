#pragma once

namespace Kuura
{
	struct Drive
	{
		std::string Path;
		std::string Description;
		uint32_t Partitions = 0;
		uint64_t Capacity = 0;
	};

	std::vector<Drive> DriveInfo();
}
