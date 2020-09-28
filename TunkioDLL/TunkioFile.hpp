#pragma once

namespace Tunkio
{
	class File
	{
	public:
		const std::string Path;

		explicit File(const std::filesystem::path& path); // For files
		explicit File(const std::string& path); // For disks
		~File();

		bool IsValid() const;
		uint64_t Size() const;
		std::pair<bool, uint64_t> Write(const uint8_t* data, const uint64_t size) const;
		bool Delete();

	private:
		uint64_t m_size = 0;

#ifdef WIN32
		HANDLE m_handle = INVALID_HANDLE_VALUE;
#else
		int m_fileDescriptor = -1;
#endif
	};
}