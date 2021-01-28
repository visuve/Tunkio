#include "../KuuraAPI-PCH.hpp"
#include "../FillConsumers/KuuraFile.hpp"
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

	File::File(const std::filesystem::path& path) :
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

		m_allocationSize = { true, fileInfo->st_blocks * 512 };

		// See notes in KuuraWin32File.cpp

		m_alignmentSize = { fileInfo->st_blksize % 512 == 0, fileInfo->st_blksize };
		m_optimalWriteSize = { fileInfo->st_blksize % 512 == 0, (fileInfo->st_blksize / 512) * 0x10000 };

		if (m_allocationSize.second % 512 != 0)
		{
			// Something is horribly wrong
			m_allocationSize.first = false;
			m_optimalWriteSize.first = false;
		}
	}

	File::File(File&& other) noexcept
	{
		std::swap(m_fileDescriptor, other.m_fileDescriptor);
		std::swap(m_allocationSize, other.m_allocationSize);
		std::swap(m_alignmentSize, other.m_alignmentSize);
		std::swap(m_optimalWriteSize, other.m_optimalWriteSize);
	}

	File& File::operator = (File&& other) noexcept
	{
		std::swap(m_fileDescriptor, other.m_fileDescriptor);
		std::swap(m_allocationSize, other.m_allocationSize);
		std::swap(m_alignmentSize, other.m_alignmentSize);
		std::swap(m_optimalWriteSize, other.m_optimalWriteSize);
		return *this;
	}

	File::~File()
	{
		if (m_fileDescriptor)
		{
			close(m_fileDescriptor);
			m_fileDescriptor = -1;
		}
	}

	bool File::IsValid() const
	{
		return m_fileDescriptor > 0;
	}

	std::pair<bool, uint64_t> File::Size() const
	{
		return m_allocationSize;
	}

	std::pair<bool, uint64_t> File::AlignmentSize() const
	{
		return m_alignmentSize;
	}

	std::pair<bool, uint64_t> File::OptimalWriteSize() const
	{
		return m_optimalWriteSize;
	}

	std::pair<bool, uint64_t> File::Write(const std::span<std::byte> data)
	{
		const ssize_t result = write(m_fileDescriptor, data.data(), data.size_bytes());

		if (result <= 0)
		{
			return { false, 0 };
		}

		uint64_t bytesWritten = static_cast<uint64_t>(result);

		return { data.size_bytes() == bytesWritten, bytesWritten };
	}

	std::pair<bool, std::vector<std::byte>> File::Read(uint64_t bytes, uint64_t offset)
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

	bool File::Flush()
	{
		return fsync(m_fileDescriptor) == 0;
	}

	bool File::Delete()
	{
		if (IsValid() && close(m_fileDescriptor))
		{
			m_fileDescriptor = -1;
			return remove(Path.c_str()) == 0;
		}

		return false;
	}
}
