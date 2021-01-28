#include "../KuuraAPI-PCH.hpp"
#include "KuuraWin32IO.hpp"

namespace Kuura
{
	OVERLAPPED Offset(uint64_t offset)
	{
		ULARGE_INTEGER cast = {};
		cast.QuadPart = offset;

		OVERLAPPED overlapped = {};
		overlapped.Offset = cast.LowPart;
		overlapped.OffsetHigh = cast.HighPart;

		return overlapped;
	}

	HANDLE Open(const std::filesystem::path& path)
	{
		// https://docs.microsoft.com/en-us/windows/win32/fileio/file-buffering
		// https://docs.microsoft.com/en-us/windows/win32/fileio/file-caching
		constexpr uint32_t DesiredAccess = GENERIC_READ | GENERIC_WRITE;
		constexpr uint32_t ShareMode = 0;
		constexpr uint32_t CreationFlags = FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH;

		return CreateFileW(
			path.c_str(),
			DesiredAccess,
			ShareMode,
			nullptr,
			OPEN_EXISTING,
			CreationFlags,
			nullptr);
	}

	std::optional<uint64_t> OptimalWriteSizeByHandle(HANDLE handle)
	{
		FILE_STORAGE_INFO storageInfo = {};

		if (!GetFileInformationByHandleEx(
			handle,
			FILE_INFO_BY_HANDLE_CLASS::FileStorageInfo,
			&storageInfo,
			sizeof(FILE_STORAGE_INFO)))
		{
			return std::nullopt;
		}

		uint64_t performanceSize = storageInfo.PhysicalBytesPerSectorForPerformance;

		if (performanceSize % 512 != 0)
		{
			return std::nullopt;
		}

		// This formula is somewhat ripped from my arse. It's loosely based on my guess about this old article:
		// https://docs.microsoft.com/en-us/previous-versions/windows/it-pro/windows-2000-server/cc938632(v=technet.10)
		// I bet that with modern NVME drives, 64KB still creates overhead.
		return performanceSize / 512 * 0x10000;
	}

	std::optional<uint64_t> AllocationSizeByHandle(HANDLE handle)
	{
		FILE_STANDARD_INFO standardInfo = {};

		if (!GetFileInformationByHandleEx(
			handle,
			FILE_INFO_BY_HANDLE_CLASS::FileStandardInfo,
			&standardInfo,
			sizeof(FILE_STANDARD_INFO)))
		{
			return std::nullopt;
		}

		return standardInfo.AllocationSize.QuadPart;
	}

	uint64_t PageSize()
	{
		SYSTEM_INFO systemInfo = {};
		GetSystemInfo(&systemInfo);

		return systemInfo.dwPageSize;
	}

	std::pair<bool, uint64_t> WriteTo(HANDLE handle, const std::span<std::byte> data)
	{
		DWORD bytesWritten = 0;
		DWORD bytesToWrite = static_cast<DWORD>(data.size_bytes());

		if (!WriteFile(handle, data.data(), bytesToWrite, &bytesWritten, nullptr))
		{
			return { false, bytesWritten };
		}

		return { bytesToWrite == bytesWritten, bytesWritten };
	}

	std::pair<bool, std::vector<std::byte>> ReadFrom(HANDLE handle, uint64_t bytes, uint64_t offset)
	{
		std::vector<std::byte> buffer(bytes);
		DWORD bytesRead = 0;
		OVERLAPPED overlapped = Offset(offset);

		if (!ReadFile(handle, buffer.data(), static_cast<DWORD>(bytes), &bytesRead, &overlapped))
		{
			return { false, std::move(buffer) };
		}

		return { bytes == bytesRead, std::move(buffer) };
	}
}
