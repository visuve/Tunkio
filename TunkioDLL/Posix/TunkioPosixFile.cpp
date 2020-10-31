#include "../PCH.hpp"
#include "../TunkioFile.hpp"

namespace Tunkio
{
	// https://linux.die.net/man/2/open
	constexpr uint32_t OpenFlags = O_WRONLY | O_DIRECT | O_SYNC;

	std::pair<bool, uint64_t> DiskSize(const int fileDescriptor)
	{
		if (!fileDescriptor)
		{
			return { false, 0 };
		}

		uint64_t size = 0;

#if defined(__linux__)
		if (ioctl(fileDescriptor, BLKGETSIZE64, &size) != 0)
#else
		if (ioctl(fileDescriptor, DIOCGMEDIASIZE, &size) != 0)
#endif
		{
			return { false, size };
		}

		return { true, size };
	}

	File::File(const std::string& path) :
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
			m_size = DiskSize(m_fileDescriptor);
		}
		else
		{
			m_size = { true, buffer.st_size };
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

	std::pair<bool, uint64_t> File::Size() const
	{
		return m_size;
	}

	std::pair<bool, uint64_t> File::Write(const uint8_t* data, const uint64_t size) const
	{
		ssize_t result = write(m_fileDescriptor, data, static_cast<size_t>(size));

		if (result == -1)
		{
			return { false, 0 };
		}

		return { true, static_cast<uint64_t>(result) };
	}

	bool File::Remove()
	{
		if (IsValid() && close(m_fileDescriptor))
		{
			m_fileDescriptor = -1;
			return remove(Path.c_str()) == 0;
		}

		return false;
	}
}