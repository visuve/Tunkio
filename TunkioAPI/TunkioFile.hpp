#pragma once

namespace Tunkio
{
	class File
	{
	public:
		const std::filesystem::path Path;

		explicit File(const std::filesystem::path& path);
		~File();

		bool IsValid() const;
		std::pair<bool, uint64_t> Size() const;
		std::pair<bool, uint64_t> Write(const void* data, const uint64_t size) const;
		bool Remove();

	private:
		std::pair<bool, uint64_t> m_size;

		bool m_isDevice = false;

#if defined(_WIN32)
		void* m_handle = reinterpret_cast<void*>(-1);
#else
		int m_fileDescriptor = -1;
#endif
	};
}