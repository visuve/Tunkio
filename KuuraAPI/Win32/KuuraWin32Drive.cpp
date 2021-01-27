#include "../KuuraAPI-PCH.hpp"
#include "../FillConsumers/KuuraDrive.hpp"
#include "KuuraWin32IO.hpp"

namespace Kuura
{
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

	std::pair<bool, uint64_t> DiskSizeByHandle(const HANDLE handle)
	{
		DWORD bytesReturned = 0;
		DISK_GEOMETRY diskGeo = {};
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


	Drive::Drive(const std::filesystem::path& path) :
		Path(path),
		m_handle(Open(path))
	{
		if (!IsValid())
		{
			return;
		}

		m_actualSize = DiskSizeByHandle(m_handle);
		m_optimalWriteSize = OptimalWriteSizeByHandle(m_handle);
		m_alignmentSize = SystemAlignmentSize();

		if (m_actualSize.second % 512 != 0)
		{
			// Something is horribly wrong
			m_actualSize.first = false;
			m_alignmentSize.first = false;
			m_optimalWriteSize.first = false;
		}
	}

	Drive::Drive(Drive&& other) noexcept
	{
		std::swap(m_handle, other.m_handle);
		std::swap(m_actualSize, other.m_actualSize);
		std::swap(m_alignmentSize, other.m_alignmentSize);
		std::swap(m_optimalWriteSize, other.m_optimalWriteSize);
	}

	Drive& Drive::operator = (Drive&& other) noexcept
	{
		std::swap(m_handle, other.m_handle);
		std::swap(m_actualSize, other.m_actualSize);
		std::swap(m_alignmentSize, other.m_alignmentSize);
		std::swap(m_optimalWriteSize, other.m_optimalWriteSize);
		return *this;
	}

	Drive::~Drive()
	{
		if (IsValid())
		{
			CloseHandle(m_handle);
			m_handle = nullptr;
		}
	}

	bool Drive::IsValid() const
	{
		return m_handle != nullptr && m_handle != INVALID_HANDLE_VALUE;
	}

	bool Drive::Unmount() const
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

	std::pair<bool, uint64_t> Drive::Size() const
	{
		return m_actualSize;
	}

	std::pair<bool, uint64_t> Drive::AlignmentSize() const
	{
		return m_alignmentSize;
	}

	std::pair<bool, uint64_t> Drive::OptimalWriteSize() const
	{
		return m_optimalWriteSize;
	}

	std::pair<bool, uint64_t> Drive::Write(const std::span<std::byte> data)
	{
		DWORD bytesWritten = 0;
		DWORD bytesToWrite = static_cast<DWORD>(data.size_bytes());

		if (!WriteFile(m_handle, data.data(), bytesToWrite, &bytesWritten, nullptr))
		{
			return { false, bytesWritten };
		}

		return { bytesToWrite == bytesWritten, bytesWritten };
	}

	std::pair<bool, std::vector<std::byte>> Drive::Read(uint64_t bytes, uint64_t offset)
	{
		std::vector<std::byte> buffer(bytes);
		DWORD bytesRead = 0;
		OVERLAPPED overlapped = Offset(offset);

		if (!ReadFile(m_handle, buffer.data(), static_cast<DWORD>(bytes), &bytesRead, &overlapped))
		{
			return { false, {} };
		}

		return { bytes == bytesRead, std::move(buffer) };
	}
};
