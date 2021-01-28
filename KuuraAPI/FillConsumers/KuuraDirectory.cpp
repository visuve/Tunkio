#include "../KuuraAPI-PCH.hpp"
#include "KuuraDirectory.hpp"

namespace Kuura
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

			auto file = std::make_shared<File>(entry);

			if (!file->IsValid())
			{
				m_files.first = false;
				return;
			}

			if (!file->Size())
			{
				m_size.first = false;
				return;
			}

			m_files.second.emplace_back(std::move(file));
			m_size.second += file->Size().value();
		}

		m_files.first = true;
		m_size.first = true;
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
		for (auto& file : m_files.second)
		{
			if (!file->Delete())
			{
				return false;
			}
		}

		return std::filesystem::remove_all(Path);
	}
}
