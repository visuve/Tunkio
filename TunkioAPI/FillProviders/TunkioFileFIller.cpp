#include "../TunkioAPI-PCH.hpp"
#include "TunkioFileFiller.hpp"
#include <fstream>

namespace Tunkio
{
	std::vector<char8_t> ReadContents(const std::filesystem::path& path)
	{
		// TODO: with big files it would be nice to have some kind of progress
		std::basic_ifstream<char8_t> stream(path, std::ios_base::binary);
		return { std::istreambuf_iterator<char8_t>(stream), std::istreambuf_iterator<char8_t>() };
	}

	FileFiller::FileFiller(const std::vector<char8_t>& content, bool verify) :
		IFillProvider(verify),
		m_fileContent(content)
	{
	}

	FileFiller::FileFiller(const std::filesystem::path& path, bool verify) :
		FileFiller(ReadContents(path), verify)
	{
	}

	FileFiller::~FileFiller()
	{
		m_fileContent.clear();
		m_fillData.clear();
	}

	bool FileFiller::HasContent()
	{
		return !m_fileContent.empty();
	}

	const void* FileFiller::Data(uint64_t bytes)
	{
		assert(HasContent());

		if (m_fillData.size() != bytes)
		{
			m_fillData.resize(bytes);

			for (size_t i = 0; i < bytes; ++i)
			{
				size_t mod = i % m_fileContent.size();
				m_fillData[i] = m_fileContent[mod];
			}
		}

		return m_fillData.data();
	}
}