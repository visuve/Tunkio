#include "../TunkioAPI-PCH.hpp"
#include "../TunkioFile.hpp"

namespace Tunkio
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

	File::File(const std::filesystem::path& path) :
		Path(path),
		m_fileDescriptor(open(path.c_str(), OpenFlags))
	{
		if (!IsValid())
		{
			return;
		}

#if defined(__linux__)
		struct stat64 buffer = {};
		if (fstat64(m_fileDescriptor, &buffer) != 0)
		{
			return;
		}
#else
		struct stat buffer = {};
		if (fstat(m_fileDescriptor, &buffer) != 0)
		{
			return;
		}
#endif
		if (S_ISBLK(buffer.st_mode) || S_ISCHR(buffer.st_mode))
		{
			m_isDevice = true;
			m_actualSize = DiskSizeByDescriptor(m_fileDescriptor);
		}
		else if (S_ISREG(buffer.st_mode))
		{
			m_actualSize = { true, buffer.st_size };
		}
		else
		{
			return;
		}

		m_allocationSize = { true, buffer.st_blocks * 512 };

		// See notes in TunkioWin32File.cpp
		m_optimalWriteSize = { buffer.st_blksize % 512 != 0, (buffer.st_blksize / 512) * 0x10000 };

		if (m_allocationSize.second % 512 != 0)
		{
			// Something is horribly wrong
			m_actualSize.first = false;
			m_allocationSize.first = false;
			m_optimalWriteSize.first = false;
		}
	}

	File::File(File&& other) noexcept
	{
		std::swap(m_fileDescriptor, other.m_fileDescriptor);
		std::swap(m_actualSize, other.m_actualSize);
		std::swap(m_allocationSize, other.m_allocationSize);
		std::swap(m_optimalWriteSize, other.m_optimalWriteSize);
		std::swap(m_isDevice, other.m_isDevice);
	}

	File& File::operator = (File&& other) noexcept
	{
		std::swap(m_fileDescriptor, other.m_fileDescriptor);
		std::swap(m_actualSize, other.m_actualSize);
		std::swap(m_allocationSize, other.m_allocationSize);
		std::swap(m_optimalWriteSize, other.m_optimalWriteSize);
		std::swap(m_isDevice, other.m_isDevice);
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

	bool File::Unmount() const
	{
		// TODO: need to check that it is actually mounted
		// return unmount(Path.c_str(), MNT_FORCE) == 0;
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

	std::pair<bool, uint64_t> File::Write(const void* data, uint64_t bytes)
	{
		const ssize_t result = write(m_fileDescriptor, data, static_cast<size_t>(bytes));

		if (result <= 0)
		{
			return { false, 0 };
		}

		uint64_t bytesWritten = static_cast<uint64_t>(result);

		return { bytesWritten == bytes, bytesWritten };
	}

	std::pair<bool, std::shared_ptr<void>> File::Read(uint64_t bytes, uint64_t offset)
	{
		std::shared_ptr<void> buffer(malloc(bytes), free);

		const ssize_t result = pread(m_fileDescriptor, buffer.get(), bytes, offset);

		if (result <= 0)
		{
			return { false, nullptr };
		}

		uint64_t bytesRead = static_cast<uint64_t>(result);

		return { bytesRead == bytes, buffer };
	}

	bool File::Flush()
	{
		return fsync(m_fileDescriptor) == 0;
	}

	bool File::Delete()
	{
		if (!m_isDevice && IsValid() && close(m_fileDescriptor))
		{
			m_fileDescriptor = -1;
			return remove(Path.c_str()) == 0;
		}

		return false;
	}
}
