#pragma once

#include "TunkioSentenceFiller.hpp"
#include <filesystem>

namespace Tunkio
{
	class FileFiller : public SentenceFiller
	{
	public:
		explicit FileFiller(
			DataUnit::Bytes bytes,
			const std::filesystem::path& path,
			bool verify);

		bool HasContent();
	};
}
