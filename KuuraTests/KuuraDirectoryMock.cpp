#include "KuuraTests-PCH.hpp"
#include "FillConsumers/KuuraDirectory.hpp"

namespace Kuura
{
	Directory::Directory(const std::filesystem::path& path) :
		Path(path)
	{
		m_files.first = true;
		m_files.second.emplace_back(std::make_shared<File>("foobar"));

		m_size.first = true;
		m_size.second = 4000;
	}

	Directory::~Directory()
	{
	}

	std::pair<bool, std::vector<std::shared_ptr<File>>>& Directory::Files()
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
