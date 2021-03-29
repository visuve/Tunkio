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
				return;
			}

			if (!file->Size())
			{
				return;
			}

			if (!m_files.has_value())
			{
				m_files.emplace();
			}

			if (!m_size.has_value())
			{
				m_size = 0;
			}

			m_files.value().emplace_back(file);
			m_size.value() += file->Size().value();
		}
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
		if (!m_files.has_value())
		{
			return false;
		}

		for (auto& file : m_files.value())
		{
			if (!file->Delete())
			{
				return false;
			}
		}

		return std::filesystem::remove_all(Path);
	}
}
