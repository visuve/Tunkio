#include "../KuuraAPI-PCH.hpp"
#include "KuuraPosixIO.hpp"

namespace Kuura
{

#if defined(__linux__)
	constexpr unsigned long int DiskSizeRequest = BLKGETSIZE64;
#else
	constexpr unsigned long int DiskSizeRequest = DIOCGMEDIASIZE;
#endif

	std::optional<FileInfo> FileInfoFromDescriptor(int descriptor)
	{
		FileInfo fileInfo = {};

		if (FileInfoFunction(descriptor, &fileInfo) != 0)
		{
			return std::nullopt;
		}

		return fileInfo;
	}

	std::pair<bool, uint64_t> WriteTo(int descriptor, const std::span<std::byte> data)
	{
		const ssize_t result = write(descriptor, data.data(), data.size_bytes());

		if (result <= 0)
		{
			return { false, 0 };
		}

		uint64_t bytesWritten = static_cast<uint64_t>(result);

		return { data.size_bytes() == bytesWritten, bytesWritten };
	}

	std::pair<bool, std::vector<std::byte>> ReadFrom(int descriptor, uint64_t bytes, uint64_t offset)
	{
		std::vector<std::byte> buffer(bytes);

		const ssize_t result = pread(descriptor, buffer.data(), bytes, offset);

		if (result <= 0)
		{
			return { false, std::move(buffer) };
		}

		uint64_t bytesRead = static_cast<uint64_t>(result);

		return { bytesRead == bytes, std::move(buffer) };
	}
}
