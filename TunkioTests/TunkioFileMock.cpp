#include "TunkioTests-PCH.hpp"
#include "../TunkioAPI/TunkioFile.hpp"

#define UNUSED(expr) do { (void)(expr); } while (0)

namespace Tunkio
{
	File::File(const std::filesystem::path& path) :
		Path(path),
		m_actualSize(true, 1),
		m_allocationSize(true, 1),
		m_optimalWriteSize(true, 1)
	{
#if defined(_WIN32)
		UNUSED(m_handle);
#else
		UNUSED(m_fileDescriptor);
#endif
		UNUSED(m_isDevice);
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
		std::shared_ptr<void> buffer(malloc(1));
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
