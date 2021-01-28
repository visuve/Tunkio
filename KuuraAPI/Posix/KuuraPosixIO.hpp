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
}
