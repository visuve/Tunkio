#include "../PCH.hpp"
#include "../TunkioFile.hpp"

namespace Tunkio
{
	HANDLE Open(const std::string& path)
	{
		// https://docs.microsoft.com/en-us/windows/win32/fileio/file-buffering
		constexpr uint32_t DesiredAccess = GENERIC_READ | GENERIC_WRITE;
		constexpr uint32_t ShareMode = FILE_SHARE_READ;
		constexpr uint32_t CreationFlags = FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH;

		return CreateFileA(
			path.c_str(),
			DesiredAccess,
			ShareMode,
			nullptr,
			OPEN_EXISTING,
			CreationFlags,
			nullptr);
	}

	std::pair<bool, uint64_t> FileSize(const HANDLE handle)
	{
		if (handle != INVALID_HANDLE_VALUE)
		{
			return { false, 0 };
		}

		LARGE_INTEGER fileSize = { 0 };

		if (!GetFileSizeEx(handle, &fileSize))
		{
			return { false, fileSize.QuadPart };
		}

		return { true, fileSize.QuadPart };
	}

	uint64_t Bytes(const DISK_GEOMETRY& diskGeo)
	{
		return diskGeo.Cylinders.QuadPart *
			diskGeo.TracksPerCylinder *
			diskGeo.SectorsPerTrack *
			diskGeo.BytesPerSector;
	}

	std::pair<bool, uint64_t> DiskSize(const HANDLE handle)
	{
		if (handle != INVALID_HANDLE_VALUE)
		{
			return { false, 0 };
		}

		unsigned long bytesReturned = 0;
		DISK_GEOMETRY diskGeo = { 0 };
		constexpr uint32_t diskGeoSize = sizeof(DISK_GEOMETRY);

		if (!DeviceIoControl(
			handle,
			IOCTL_DISK_GET_DRIVE_GEOMETRY,
			nullptr,
			0,
			&diskGeo,
			diskGeoSize,
			&bytesReturned,
			nullptr))
		{
			return { false, Bytes(diskGeo) };
		}

		return { bytesReturned == sizeof(DISK_GEOMETRY), Bytes(diskGeo) };
	}

	File::File(const std::filesystem::path& path) :
		Path(path.string()),
		m_handle(Open(path.string()))
	{
		if (!IsValid())
		{
			return;
		}

		m_size = FileSize(m_handle);
	}

	File::File(const std::string& path) :
		Path(path),
		m_handle(Open(path))
	{
		if (!IsValid())
		{
			return;
		}

		m_size = DiskSize(m_handle);
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
		return m_handle != nullptr && m_handle != INVALID_HANDLE_VALUE;
	}

	std::pair<bool, uint64_t> File::Size() const
	{
		return m_size;
	}

	std::pair<bool, uint64_t> File::Write(const uint8_t* data, const uint64_t size) const
	{
		DWORD bytesWritten = 0;

		if (!WriteFile(m_handle, data, static_cast<DWORD>(size), &bytesWritten, nullptr))
		{
			return { false, bytesWritten };
		}

		return { true, bytesWritten };
	}

	bool File::Remove()
	{
		if (IsValid() && CloseHandle(m_handle))
		{
			m_handle = nullptr;
			return DeleteFileA(Path.c_str());
		}

		return false;
	}
};
