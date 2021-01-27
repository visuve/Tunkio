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

	std::pair<bool, uint64_t> OptimalWriteSizeByHandle(const HANDLE handle)
	{
		FILE_STORAGE_INFO storageInfo = {};

		if (!GetFileInformationByHandleEx(
			handle,
			FILE_INFO_BY_HANDLE_CLASS::FileStorageInfo,
			&storageInfo,
			sizeof(FILE_STORAGE_INFO)))
		{
			return { false, 0 };
		}

		uint64_t performanceSize = storageInfo.PhysicalBytesPerSectorForPerformance;

		if (performanceSize % 512 != 0)
		{
			return { false, performanceSize };
		}

		// This formula is somewhat ripped from my arse. It's loosely based on my guess about this old article:
		// https://docs.microsoft.com/en-us/previous-versions/windows/it-pro/windows-2000-server/cc938632(v=technet.10)
		// I bet that with modern NVME drives, 64KB still creates overhead.
		return { true, (performanceSize / 512) * 0x10000 };
	}

	std::pair<bool, uint64_t> AllocationSizeByHandle(const HANDLE handle)
	{
		FILE_STANDARD_INFO standardInfo = {};

		if (!GetFileInformationByHandleEx(
			handle,
			FILE_INFO_BY_HANDLE_CLASS::FileStandardInfo,
			&standardInfo,
			sizeof(FILE_STANDARD_INFO)))
		{
			return { false, 0 };
		}

		return { true, standardInfo.AllocationSize.QuadPart };
	}

	std::pair<bool, uint64_t> SystemAlignmentSize()
	{
		SYSTEM_INFO systemInfo = {};
		GetSystemInfo(&systemInfo);

		return { true, systemInfo.dwPageSize };
	}
}
