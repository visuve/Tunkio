#pragma once

#include "TunkioFillProvider.hpp"

namespace Tunkio
{
	class FileFiller : public IFillProvider
	{
	public:
		explicit FileFiller(const std::vector<char8_t>& content, bool verify);
		explicit FileFiller(const std::filesystem::path& path, bool verify);
		~FileFiller();

		bool HasContent();
		const void* Data(uint64_t bytes) override;

	private:
		const std::vector<char8_t> m_fileContent;
		std::vector<char8_t> m_fillData;
		size_t m_offset = 0;
	};
}
