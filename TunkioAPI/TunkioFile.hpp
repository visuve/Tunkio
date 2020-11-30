#pragma once

#include <filesystem>
#include <utility>

namespace Tunkio
{
	class File
	{
	public:
		const std::filesystem::path Path;

		explicit File(const std::filesystem::path& path);
		~File();

		bool IsValid() const;
		bool Unmount() const;
		const std::pair<bool, uint64_t>& ActualSize() const;
		const std::pair<bool, uint64_t>& AllocationSize() const;
		const std::pair<bool, uint64_t>& OptimalWriteSize() const;
		std::pair<bool, uint64_t> Write(const void* data, uint64_t bytes);
		std::pair<bool, uint64_t> Write(const void* data, uint64_t bytes, uint64_t offset);
		std::pair<bool, std::shared_ptr<void>> Read(uint64_t bytes);
		std::pair<bool, std::shared_ptr<void>> Read(uint64_t bytes, uint64_t offset);
		bool Flush();
		bool Rewind();
		bool Delete();

	private:
		std::pair<bool, uint64_t> m_actualSize;
		std::pair<bool, uint64_t> m_allocationSize;
		std::pair<bool, uint64_t> m_optimalWriteSize;

		bool m_isDevice = false;

#if defined(_WIN32)
		void* m_handle = reinterpret_cast<void*>(-1);
#else
		int m_fileDescriptor = -1;
#endif
	};
}
