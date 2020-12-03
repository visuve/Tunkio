#include "TunkioTests-PCH.hpp"
#include "../TunkioAPI/TunkioFile.hpp"

namespace Tunkio
{
	File::File(const std::filesystem::path& path) :
		Path(path),
		m_actualSize(true, 1234),
		m_allocationSize(true, 4096),
		m_optimalWriteSize(true, 1024)
	{
	}

	File::File(File&& other) noexcept
	{
#if defined(_WIN32)
		std::swap(m_handle, other.m_handle);
#else
		std::swap(m_fileDescriptor, other.m_fileDescriptor);
#endif
		std::swap(m_actualSize, other.m_actualSize);
		std::swap(m_allocationSize, other.m_allocationSize);
		std::swap(m_optimalWriteSize, other.m_optimalWriteSize);
		std::swap(m_isDevice, other.m_isDevice);
	}

	File& File::operator = (File&& other) noexcept
	{
#if defined(_WIN32)
		std::swap(m_handle, other.m_handle);
#else
		std::swap(m_fileDescriptor, other.m_fileDescriptor);
#endif
		std::swap(m_actualSize, other.m_actualSize);
		std::swap(m_allocationSize, other.m_allocationSize);
		std::swap(m_optimalWriteSize, other.m_optimalWriteSize);
		std::swap(m_isDevice, other.m_isDevice);
		return *this;
	}

	File::~File()
	{
	}

	bool File::IsValid() const
	{
		return true;
	}

	bool File::Unmount() const
	{
		return true;
	}

	const std::pair<bool, uint64_t>& File::ActualSize() const
	{
		return m_actualSize;
	}

	const std::pair<bool, uint64_t>& File::AllocationSize() const
	{
		return m_allocationSize;
	}

	const std::pair<bool, uint64_t>& File::OptimalWriteSize() const
	{
		return m_optimalWriteSize;
	}

	std::pair<bool, uint64_t> File::Write(const std::span<std::byte> data)
	{
		return { !data.empty(), data.size() };
	}

	std::pair<bool, std::vector<std::byte>> File::Read(uint64_t size, uint64_t)
	{
		return { size > 0, std::vector<std::byte>(1, std::byte(0xFF)) };
	}

	bool File::Flush()
	{
		return true;
	}

	bool File::Delete()
	{
		return true;
	}
}
