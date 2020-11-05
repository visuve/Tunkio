#pragma once

namespace Tunkio
{
	struct Drive
	{
		std::string Path;
		std::string Description;
		uint32_t Partitions;
		uint64_t Capacity;
	};

	std::vector<Drive> DriveInfo();
}