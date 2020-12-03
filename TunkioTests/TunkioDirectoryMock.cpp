#include "TunkioTests-PCH.hpp"
#include "../TunkioAPI/TunkioDirectory.hpp"

namespace Tunkio
{
	Directory::Directory(const std::filesystem::path& path) :
		Path(path)
	{
		m_files.first = true;
		m_files.second.emplace_back("foobar");

		m_size.first = true;
		m_size.second = 4000;
	}

	Directory::~Directory()
	{
	}

	std::pair<bool, std::vector<File>>& Directory::Files()
	{
		return m_files;
	}

	std::pair<bool, uint64_t> Directory::Size() const
	{
		return m_size;
	}

	bool Directory::RemoveAll()
	{
		return true;
	}
}
