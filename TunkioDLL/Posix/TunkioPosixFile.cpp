#include "../PCH.hpp"
#include "../TunkioFile.hpp"

namespace Tunkio
{
	int Open(const std::string& path)
	{
		// https://linux.die.net/man/2/open
		constexpr uint32_t Flags = O_WRONLY | O_DIRECT | O_LARGEFILE | O_SYNC;
		return open(path.c_str(), Flags);
	}

	uint64_t FileSize(const int fileDescriptor)
	{
		struct stat64 buffer = { 0 };

		if (fstat64(fileDescriptor, &buffer) != 0)
		{
			return false;
		}

		return buffer.st_size;
	}

	uint64_t DiskSize(const int fileDescriptor)
	{
		if (!fileDescriptor)
		{
			return 0;
		}

		uint64_t size = 0;

		if (ioctl(fileDescriptor, BLKGETSIZE64, &size) != 0)
		{
			return 0;
		}

		return size;
	}

	File::File(const std::filesystem::path& path) :
		Path(path.string()),
		m_fileDescriptor(Open(path.string()))
	{
		if (!IsValid())
		{
			return;
		}

		m_size = FileSize(m_fileDescriptor);
	}

	File::File(const std::string& path) :
		Path(path),
		m_fileDescriptor(Open(path))
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

	uint64_t File::Size() const
	{
		return m_size;
	}

	std::pair<bool, uint64_t> File::Write(const uint8_t* data, const uint32_t size) const
	{
		ssize_t result = write(m_fileDescriptor, data, size);

		if (result == -1)
		{
			return { false, 0 };
		}

		return { true, static_cast<uint64_t>(result) };
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
};
