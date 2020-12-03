#pragma once

#include "TunkioFillProvider.hpp"

namespace Tunkio
{
	class FileFiller : public IFillProvider
	{
	public:
		explicit FileFiller(const std::vector<std::byte>& content, bool verify);
		explicit FileFiller(const std::filesystem::path& path, bool verify);
		~FileFiller() = default;

		bool HasContent();
		std::span<std::byte> Data(uint64_t bytes) override;

	private:
		const std::vector<std::byte> m_fileContent;
		size_t m_offset = 0;
	};
}
