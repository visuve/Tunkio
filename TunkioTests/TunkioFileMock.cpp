#include "TunkioTests-PCH.hpp"
#include "../TunkioAPI/TunkioFile.hpp"

#define UNUSED(expr) do { (void)(expr); } while (0)

namespace Tunkio
{
	File::File(const std::filesystem::path& path) :
		Path(path)
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

	std::pair<bool, uint64_t> File::Size() const
	{
		return { true, 1 };
	}

	std::pair<bool, uint64_t> File::Write(const void* data, const uint64_t size) const
	{
		return { data, size };
	}

	bool File::Remove()
	{
		return true;
	}
}
