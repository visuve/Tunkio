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
		struct stat64 buffer = { };
		if (fstat64(m_fileDescriptor, &buffer) != 0)
		{
			return;
		}
#else
		struct stat buffer = { };
		if (fstat(m_fileDescriptor, &buffer) != 0)
		{
			return;
		}
#endif
		if ((buffer.st_mode & S_IFMT) == S_IFBLK)
		{
			m_isDevice = true;
			m_actualSize = DiskSizeByDescriptor(m_fileDescriptor);
		}
		else
		{
			m_actualSize = { true, buffer.st_size };
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
		// TODO!
		return true;
	}

	std::pair<bool, uint64_t> File::Size() const
	{
		return m_size;
	}

	std::pair<bool, uint64_t> File::Write(const void* data, const uint64_t size) const
	{
		const ssize_t result = write(m_fileDescriptor, data, static_cast<size_t>(size));

		if (result == -1)
		{
			return { false, 0 };
		}

		if (m_isDevice)
		{
			return { true, static_cast<uint64_t>(result) };
		}

		return { fsync(m_fileDescriptor) == 0, static_cast<uint64_t>(result) };
	}

	bool File::Remove()
	{
		if (!m_isDevice && IsValid() && close(m_fileDescriptor))
		{
			m_fileDescriptor = -1;
			return remove(Path.c_str()) == 0;
		}

		return false;
	}
}
