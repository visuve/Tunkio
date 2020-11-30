#include "TunkioDirectory.hpp"
#include "TunkioAPI-PCH.hpp"
#include "TunkioDirectory.hpp"

namespace Tunkio
{
	Directory::Directory(const std::filesystem::path& path) :
		Path(path)
	{
		for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
		{
			if (!entry.is_regular_file())
			{
				continue;
			}

			File file(entry);

			if (!file.IsValid())
			{
				m_files.first = false;
				return;
			}

			if (!file.AllocationSize().first)
			{
				m_size.first = false;
				return;
			}

			m_files.second.emplace_back(std::move(file));
			m_size.second += file.AllocationSize().second;
		}

		m_files.first = true;
		m_size.first = true;
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
		for (File& file : m_files.second)
		{
			if (!file.Delete())
			{
				return false;
			}
		}

		return std::filesystem::remove_all(Path);
	}
}
