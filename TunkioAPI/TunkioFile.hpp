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
		File(File&&) noexcept;
		File& operator = (File&&) noexcept;
		File(const File&) = delete;
		File& operator = (const File&) = delete;
		~File();

		bool IsValid() const;
		bool Unmount() const;
		const std::pair<bool, uint64_t>& ActualSize() const;
		const std::pair<bool, uint64_t>& AllocationSize() const;
		const std::pair<bool, uint64_t>& OptimalWriteSize() const;
		std::pair<bool, uint64_t> Write(std::span<std::byte> data, uint64_t bytes);
		std::pair<bool, std::vector<std::byte>> Read(uint64_t bytes, uint64_t offset);
		bool Flush();
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
