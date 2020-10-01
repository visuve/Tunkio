#include "../PCH.hpp"
#include "../TunkioFile.hpp"

namespace Tunkio
{
	// https://linux.die.net/man/2/open
	constexpr uint32_t OpenFlags = O_WRONLY | O_DIRECT | O_SYNC;

	std::pair<bool, uint64_t> FileSize(const int fileDescriptor)
	{
#if defined(__linux__)
		struct stat64 buffer = { 0 };
		if (fstat64(fileDescriptor, &buffer) != 0)
#else
		struct stat buffer = { 0 };
		if (fstat(fileDescriptor, &buffer) != 0)
#endif
		{
			return { false, buffer.st_size };
		}

		return { true, buffer.st_size };
	}

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

	File::File(const std::filesystem::path& path) :
		Path(path.string()),
		m_fileDescriptor(open(path.c_str(), OpenFlags))
	{
		if (!IsValid())
		{
			return;
		}

		m_size = FileSize(m_fileDescriptor);
	}

	File::File(const std::string& path) :
		Path(path),
		m_fileDescriptor(open(path.c_str(), OpenFlags))
	{
		if (!IsValid())
		{
			return;
		}

		m_size = DiskSize(m_fileDescriptor);
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
};