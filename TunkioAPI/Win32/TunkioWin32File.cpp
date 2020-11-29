#include "../TunkioAPI-PCH.hpp"
#include "../TunkioFile.hpp"

namespace Tunkio
{
	HANDLE Open(const std::filesystem::path& path)
	{
		// https://docs.microsoft.com/en-us/windows/win32/fileio/file-buffering
		// https://docs.microsoft.com/en-us/windows/win32/fileio/file-caching
		constexpr uint32_t DesiredAccess = GENERIC_READ | GENERIC_WRITE;
		constexpr uint32_t ShareMode = 0;
		constexpr uint32_t CreationFlags =
			FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH;

		return CreateFileW(
			path.c_str(),
			DesiredAccess,
			ShareMode,
			nullptr,
			OPEN_EXISTING,
			CreationFlags,
			nullptr);
	}

	uint64_t Bytes(const DISK_GEOMETRY& diskGeo)
	{
		assert(diskGeo.Cylinders.QuadPart);
		assert(diskGeo.TracksPerCylinder);
		assert(diskGeo.SectorsPerTrack);
		assert(diskGeo.BytesPerSector);

		return diskGeo.Cylinders.QuadPart *
			diskGeo.TracksPerCylinder *
			diskGeo.SectorsPerTrack *
			diskGeo.BytesPerSector;
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

	std::pair<bool, uint64_t> DiskSizeByHandle(const HANDLE handle)
	{
		DWORD bytesReturned = 0;
		DISK_GEOMETRY diskGeo = { };
		constexpr uint32_t DiskGeoSize = sizeof(DISK_GEOMETRY);

		if (!DeviceIoControl(
			handle,
			IOCTL_DISK_GET_DRIVE_GEOMETRY,
			nullptr,
			0,
			&diskGeo,
			DiskGeoSize,
			&bytesReturned,
			nullptr))
		{
			return { false, 0 };
		}

		assert(bytesReturned == DiskGeoSize);

		return { true, Bytes(diskGeo) };
	}

	File::File(const std::filesystem::path& path) :
		Path(path),
		m_handle(Open(path))
	{
		if (!IsValid())
		{
			return;
		}

		BY_HANDLE_FILE_INFORMATION fileInfo = {};

		if (!GetFileInformationByHandle(m_handle, &fileInfo))
		{
			if (GetLastError() == ERROR_INVALID_FUNCTION &&
				GetFileType(m_handle) == FILE_TYPE_DISK)
			{
				// This is just a guess, but I cannot come up a better way
				fileInfo.dwFileAttributes = FILE_ATTRIBUTE_DEVICE;
			}
		}

		if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DEVICE)
		{
			m_isDevice = true;
			m_actualSize = DiskSizeByHandle(m_handle);
		}
		else if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN ||
			fileInfo.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE ||
			fileInfo.dwFileAttributes & FILE_ATTRIBUTE_NORMAL ||
			fileInfo.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY)
		{
			m_actualSize.first = true;
			m_actualSize.second = fileInfo.nFileSizeHigh;
			m_actualSize.second <<= 32;
			m_actualSize.second |= fileInfo.nFileSizeLow;
		}
		else
		{
			return;
		}

		m_optimalWriteSize = OptimalWriteSizeByHandle(m_handle);
		m_allocationSize = AllocationSizeByHandle(m_handle);

		if (m_allocationSize.second % 512 != 0)
		{
			// Something is horribly wrong
			m_actualSize.first = false;
			m_allocationSize.first = false;
			m_optimalWriteSize.first = false;
		}
	}

	File::~File()
	{
		if (IsValid())
		{
			CloseHandle(m_handle);
			m_handle = nullptr;
		}
	}

	bool File::IsValid() const
	{
		return m_handle != nullptr && m_handle != INVALID_HANDLE_VALUE;
	}

	bool File::Unmount() const
	{
		if (!DeviceIoControl(
			m_handle,
			IOCTL_DISK_DELETE_DRIVE_LAYOUT,
			nullptr,
			0,
			nullptr,
			0,
			nullptr,
			nullptr))
		{
			return false;
		}

		return true;
	}

	const std::pair<bool, uint64_t>& File::ActualSize() const
	{
		return m_actualSize;
	}

	const std::pair<bool, uint64_t>& File::AllocationSize() const
	{
		return m_allocationSize;
	}

	const std::pair<bool, uint64_t>& File::OptimalWriteSize() const
	{
		return m_optimalWriteSize;
	}

	std::pair<bool, uint64_t> File::Write(const void* data, const uint64_t size) const
	{
		DWORD bytesWritten = 0;

		if (!WriteFile(m_handle, data, static_cast<DWORD>(size), &bytesWritten, nullptr))
		{
			return { false, bytesWritten };
		}

		if (m_isDevice)
		{
			return { true, bytesWritten };
		}

		return { FlushFileBuffers(m_handle), bytesWritten };
	}

	bool File::Rewind()
	{
		const LARGE_INTEGER beginning = {};
		LARGE_INTEGER newPosition = {};

		if (!SetFilePointerEx(m_handle, beginning, &newPosition, FILE_BEGIN))
		{
			return false;
		}

		return newPosition.QuadPart == 0;
	}

	bool File::Delete()
	{
		if (!m_isDevice && IsValid() && CloseHandle(m_handle))
		{
			m_handle = nullptr;
			return DeleteFileW(Path.c_str());
		}

		return false;
	}
};
