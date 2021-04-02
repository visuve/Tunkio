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

			if (!_files.has_value())
			{
				_files.emplace();
			}

			if (!_size.has_value())
			{
				_size = 0;
			}

			_files.value().emplace_back(file);
			_size.value() += file->Size().value();
		}
	}

	Directory::~Directory()
	{
	}

	std::optional<std::vector<std::shared_ptr<File>>>& Directory::Files()
	{
		return _files;
	}

	std::optional<uint64_t> Directory::Size() const
	{
		return _size;
	}

	bool Directory::RemoveAll()
	{
		if (!_files.has_value())
		{
			return false;
		}

		for (auto& file : _files.value())
		{
			if (!file->Delete())
			{
				return false;
			}
		}

		return std::filesystem::remove_all(Path);
	}
}
