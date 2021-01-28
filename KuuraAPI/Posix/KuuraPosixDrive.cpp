#include "../KuuraAPI-PCH.hpp"
#include "../FillConsumers/KuuraDrive.hpp"
#include "KuuraPosixIO.hpp"

namespace Kuura
{
#if defined(__linux__)
	constexpr unsigned long int DiskSizeRequest = BLKGETSIZE64;
#else
	constexpr unsigned long int DiskSizeRequest = DIOCGMEDIASIZE;
#endif

	// https://linux.die.net/man/2/open
	constexpr uint32_t OpenFlags = O_RDWR | O_DIRECT | O_SYNC;

	std::pair<bool, uint64_t> DiskSizeByDescriptor(const int descriptor)
	{
		uint64_t size = 0;

		if (ioctl(descriptor, DiskSizeRequest, &size) != 0)
		{
			return { false, size };
		}

		return { true, size };
	}

	Drive::Drive(const std::filesystem::path& path) :
		Path(path),
		m_fileDescriptor(open(path.c_str(), OpenFlags))
	{
		if (!IsValid())
		{
			return;
		}

		std::optional<FileInfo> fileInfo =
			FileInfoFromDescriptor(m_fileDescriptor);

		if (!fileInfo.has_value())
		{
			return;
		}

		m_actualSize = DiskSizeByDescriptor(m_fileDescriptor);

		// See notes in KuuraWin32File.cpp

		m_alignmentSize = { fileInfo->st_blksize % 512 == 0, fileInfo->st_blksize };
		m_optimalWriteSize = { fileInfo->st_blksize % 512 == 0, (fileInfo->st_blksize / 512) * 0x10000 };

		if (fileInfo->st_blksize % 512 != 0)
		{
			// Something is horribly wrong
			m_actualSize.first = false;
			m_optimalWriteSize.first = false;
		}
	}

	Drive::Drive(Drive&& other) noexcept
	{
		std::swap(m_fileDescriptor, other.m_fileDescriptor);
		std::swap(m_actualSize, other.m_actualSize);
		std::swap(m_alignmentSize, other.m_alignmentSize);
		std::swap(m_optimalWriteSize, other.m_optimalWriteSize);
	}

	Drive& Drive::operator = (Drive&& other) noexcept
	{
		std::swap(m_fileDescriptor, other.m_fileDescriptor);
		std::swap(m_actualSize, other.m_actualSize);
		std::swap(m_alignmentSize, other.m_alignmentSize);
		std::swap(m_optimalWriteSize, other.m_optimalWriteSize);
		return *this;
	}

	Drive::~Drive()
	{
		if (m_fileDescriptor)
		{
			close(m_fileDescriptor);
			m_fileDescriptor = -1;
		}
	}

	bool Drive::IsValid() const
	{
		return m_fileDescriptor > 0;
	}

	bool Drive::Unmount() const
	{
		// TODO: need to check that it is actually mounted
		// return unmount(Path.c_str(), MNT_FORCE) == 0;
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
		const ssize_t result = write(m_fileDescriptor, data.data(), data.size_bytes());

		if (result <= 0)
		{
			return { false, 0 };
		}

		uint64_t bytesWritten = static_cast<uint64_t>(result);

		return { data.size_bytes() == bytesWritten, bytesWritten };
	}

	std::pair<bool, std::vector<std::byte>> Drive::Read(uint64_t bytes, uint64_t offset)
	{
		std::vector<std::byte> buffer;

		const ssize_t result = pread(m_fileDescriptor, buffer.data(), bytes, offset);

		if (result <= 0)
		{
			return { false, {} };
		}

		uint64_t bytesRead = static_cast<uint64_t>(result);

		return { bytesRead == bytes, std::move(buffer) };
	}
}
