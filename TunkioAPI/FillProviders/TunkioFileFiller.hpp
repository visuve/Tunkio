#pragma once

#include "TunkioFillProvider.hpp"

namespace Tunkio
{
	class FileFiller : public IFillProvider
	{
	public:
		explicit FileFiller(
			DataUnit::Bytes bytes,
			const std::filesystem::path& path,
			bool verify);

		bool HasContent();
		const void* Data() override;

	private:
		std::vector<uint8_t> m_fileContent;
		std::vector<uint8_t> m_fillData;
	};
}
