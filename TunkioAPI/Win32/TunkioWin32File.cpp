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

	constexpr bool IsValidHandle(const HANDLE handle)
	{
		return handle != nullptr && handle != INVALID_HANDLE_VALUE;
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

	std::pair<bool, uint64_t> DiskSize(const HANDLE handle)
	{
		if (!IsValidHandle(handle))
		{
			return { false, 0 };
		}

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

		BY_HANDLE_FILE_INFORMATION info = {};

		if (!GetFileInformationByHandle(m_handle, &info))
		{
			if (GetLastError() == ERROR_INVALID_FUNCTION && GetFileType(m_handle) == FILE_TYPE_DISK)
			{
				// This is just a guess, but I cannot come up a better way
				info.dwFileAttributes = FILE_ATTRIBUTE_DEVICE;
			}
		}

		if (info.dwFileAttributes & FILE_ATTRIBUTE_DEVICE)
		{
			m_isDevice = true;
			m_size = DiskSize(m_handle);
		}
		else if (info.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN ||
			info.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE ||
			info.dwFileAttributes & FILE_ATTRIBUTE_NORMAL ||
			info.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY)
		{
			m_size.first = true;
			m_size.second = info.nFileSizeHigh;
			m_size.second <<= 32;
			m_size.second |= info.nFileSizeLow;
		}
	}

	File::~File()
	{
		if (m_handle)
		{
			CloseHandle(m_handle);
			m_handle = nullptr;
		}
	}

	bool File::IsValid() const
	{
		return IsValidHandle(m_handle);
	}

	std::pair<bool, uint64_t> File::Size() const
	{
		return m_size;
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

	bool File::Remove()
	{
		if (!m_isDevice && IsValid() && CloseHandle(m_handle))
		{
			m_handle = nullptr;
			return DeleteFileW(Path.c_str());
		}

		return false;
	}
};
