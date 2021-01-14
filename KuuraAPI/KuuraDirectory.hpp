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
		std::pair<bool, std::vector<File>>& Files();
		std::pair<bool, uint64_t> Size() const;

		bool RemoveAll();

	private:
		std::pair<bool, std::vector<File>> m_files;
		std::pair<bool, uint64_t> m_size;
	};
}
