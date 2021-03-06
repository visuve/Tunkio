#pragma once

#include "KuuraFillProvider.hpp"

namespace Kuura
{
	class FileFiller : public IFillProvider
	{
	public:
		explicit FileFiller(const std::vector<std::byte>& content, bool verify);
		explicit FileFiller(const std::filesystem::path& path, bool verify);
		~FileFiller() = default;

		bool HasContent();
		std::span<std::byte> Data(uint64_t bytes, uint64_t alignment) override;

	private:
		const std::vector<std::byte> _fileContent;
		size_t _offset = 0;
	};
}
