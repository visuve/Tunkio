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

	std::optional<uint64_t> DiskSizeByDescriptor(const int descriptor)
	{
		uint64_t size = 0;

		if (ioctl(descriptor, DiskSizeRequest, &size) != 0)
		{
			return std::nullopt;
		}

		return size;
	}

	Drive::Drive(const std::filesystem::path& path) :
		IFillConsumer(path),
		m_descriptor(open(path.c_str(), OpenFlags))
	{
		if (!IsValid())
		{
			return;
		}

		std::optional<FileInfo> fileInfo =
			FileInfoFromDescriptor(m_descriptor);

		if (!fileInfo)
		{
			return;
		}

		m_actualSize = DiskSizeByDescriptor(m_descriptor);

		// See notes in KuuraWin32File.cpp

		m_alignmentSize = fileInfo->st_blksize;
		m_optimalWriteSize = fileInfo->st_blksize / 512 * 0x10000;

		if (m_alignmentSize.value() % 512 != 0)
		{
			// Something is horribly wrong

			m_actualSize = std::nullopt;
			m_alignmentSize = std::nullopt;
			m_optimalWriteSize = std::nullopt;
		}
	}

	Drive::Drive(Drive&& other) noexcept :
		IFillConsumer("")
	{
		*this = std::move(other);
	}

	Drive& Drive::operator = (Drive&& other) noexcept
	{
		if (this != &other)
		{
			std::swap(Path, other.Path);
			std::swap(m_descriptor, other.m_descriptor);
			std::swap(m_actualSize, other.m_actualSize);
			std::swap(m_alignmentSize, other.m_alignmentSize);
			std::swap(m_optimalWriteSize, other.m_optimalWriteSize);
		}

		return *this;
	}

	Drive::~Drive()
	{
		if (m_descriptor)
		{
			close(m_descriptor);
			m_descriptor = -1;
		}
	}

	bool Drive::IsValid() const
	{
		return m_descriptor > 0;
	}

	bool Drive::Unmount() const
	{
		// TODO: need to check that it is actually mounted
		// return unmount(Path.c_str(), MNT_FORCE) == 0;
		return true;
	}

	std::optional<uint64_t> Drive::Size() const
	{
		return m_actualSize;
	}

	std::optional<uint64_t> Drive::AlignmentSize() const
	{
		return m_alignmentSize;
	}

	std::optional<uint64_t> Drive::OptimalWriteSize() const
	{
		return m_optimalWriteSize;
	}

	std::pair<bool, uint64_t> Drive::Write(const std::span<std::byte> data)
	{
		return WriteTo(m_descriptor, data);
	}

	std::pair<bool, std::vector<std::byte>> Drive::Read(uint64_t bytes, uint64_t offset)
	{
		return ReadFrom(m_descriptor, bytes, offset);
	}
}
