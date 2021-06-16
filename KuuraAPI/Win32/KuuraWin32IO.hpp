#pragma once

namespace Kuura
{
	OVERLAPPED Offset(uint64_t offset);
	HANDLE Open(const std::filesystem::path& path);
	std::optional<uint64_t> OptimalWriteSizeByHandle(HANDLE handle);
	std::optional<uint64_t> AllocationSizeByHandle(HANDLE handle);

	std::pair<bool, uint64_t> WriteTo(HANDLE handle, const std::span<std::byte> data);
	std::pair<bool, std::vector<std::byte>> ReadFrom(HANDLE handle, uint64_t bytes, uint64_t offset);
}
