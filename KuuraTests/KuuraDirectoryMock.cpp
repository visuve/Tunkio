#include "KuuraTests-PCH.hpp"
#include "FillConsumers/KuuraDirectory.hpp"

namespace Kuura
{
	Directory::Directory(const std::filesystem::path& path) :
		Path(path)
	{
		m_files.emplace().emplace_back(std::make_shared<File>("foobar"));
		m_size = 4000;
	}

	Directory::~Directory()
	{
	}

	std::optional<std::vector<std::shared_ptr<File>>>& Directory::Files()
	{
		return m_files;
	}

	std::optional<uint64_t> Directory::Size() const
	{
		return m_size;
	}

	bool Directory::RemoveAll()
	{
		return true;
	}
}
