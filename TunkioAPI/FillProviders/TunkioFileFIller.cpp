#include "TunkioFileFiller.hpp"
#include <fstream>

namespace Tunkio
{
	std::string ReadContents(const std::filesystem::path& path)
	{
		// TODO: with big files it would be nice to have some kind of progress
		std::ifstream stream(path, std::ios_base::binary);
		return { std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>() };
	}

	FileFiller::FileFiller(DataUnit::Bytes bytes, const std::filesystem::path& path, bool verify) :
		SentenceFiller(bytes, ReadContents(path), "\0", verify)
	{
	}

	bool FileFiller::HasContent()
	{
		return m_fillString.size() > 0;
	}
}