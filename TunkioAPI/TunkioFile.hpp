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
		bool Unmount() const;
		std::pair<bool, uint64_t> ActualSize() const;
		std::pair<bool, uint64_t> AllocationSize() const;
		std::pair<bool, uint64_t> OptimalWriteSize() const;
		std::pair<bool, uint64_t> Write(const void* data, const uint64_t size) const;
		bool Remove();

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