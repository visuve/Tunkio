#pragma once

namespace Tunkio
{
	class File
	{
	public:
		const std::string Path;

		explicit File(const std::string& path);
		~File();

		bool IsValid() const;
		std::pair<bool, uint64_t> Size() const;
		std::pair<bool, uint64_t> Write(const void* data, const uint64_t size) const;
		bool Remove();

	private:
		std::pair<bool, uint64_t> m_size;

#ifdef WIN32
		HANDLE m_handle = INVALID_HANDLE_VALUE;
#else
		int m_fileDescriptor = -1;
#endif
	};
}