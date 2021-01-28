#pragma once

#include <optional>

namespace Kuura
{
#if defined(__linux__)
	using FileInfo = struct stat64;
	constexpr auto FileInfoFunction = fstat64;
#else
	using FileInfo = struct stat;
	constexpr auto FileInfoFunction = fstat;
#endif

	std::optional<FileInfo> FileInfoFromDescriptor(int descriptor);

	std::pair<bool, uint64_t> WriteTo(int descriptor, const std::span<std::byte> data);
	std::pair<bool, std::vector<std::byte>> ReadFrom(int descriptor, uint64_t bytes, uint64_t offset);
}
