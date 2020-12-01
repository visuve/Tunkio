#include "TunkioTests-PCH.hpp"
#include "../TunkioAPI/TunkioFile.hpp"

namespace Tunkio
{
	File::File(const std::filesystem::path& path) :
		Path(path),
		m_actualSize(true, 1),
		m_allocationSize(true, 1),
		m_optimalWriteSize(true, 1)
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

	std::pair<bool, uint64_t> File::Write(const void* data, uint64_t size, uint64_t)
	{
		return { data != nullptr, size };
	}

	std::pair<bool, std::shared_ptr<void>> File::Read(uint64_t size, uint64_t)
	{
		std::shared_ptr<void> buffer(malloc(1), free);
		return { size > 0, buffer };
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
