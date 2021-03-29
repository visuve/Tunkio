#pragma once

#include "KuuraFile.hpp"

#include <vector>

namespace Kuura
{
	class Directory
	{
	public:
		const std::filesystem::path Path;

		Directory(const std::filesystem::path& path);
		~Directory();
		std::optional<std::vector<std::shared_ptr<File>>>& Files();
		std::optional<uint64_t> Size() const;

		bool RemoveAll();

	private:
		std::optional<std::vector<std::shared_ptr<File>>> m_files;
		std::optional<uint64_t> m_size;
	};
}
