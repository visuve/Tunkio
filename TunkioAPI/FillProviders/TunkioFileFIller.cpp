#include "../TunkioAPI-PCH.hpp"
#include "TunkioFileFiller.hpp"
#include <fstream>

namespace Tunkio
{
	std::vector<std::byte> ReadContents(const std::filesystem::path& path)
	{
		// TODO: with big files it would be nice to have some kind of progress
		std::basic_ifstream<std::byte> stream(path, std::ios_base::binary);
		return
		{
			std::istreambuf_iterator<std::byte>(stream),
			std::istreambuf_iterator<std::byte>()
		};
	}

	FileFiller::FileFiller(const std::vector<std::byte>& content, bool verify) :
		IFillProvider(verify),
		m_fileContent(content)
	{
	}

	FileFiller::FileFiller(const std::filesystem::path& path, bool verify) :
		FileFiller(ReadContents(path), verify)
	{
	}

	bool FileFiller::HasContent()
	{
		return !m_fileContent.empty();
	}

	std::span<std::byte> FileFiller::Data(uint64_t bytes, uint64_t alignment)
	{
		assert(HasContent());

		if (m_fillData.size() != bytes)
		{
			m_fillData.resize(bytes);
			AlignData(alignment);
		}

		for (size_t i = 0; i < bytes; ++i)
		{
			m_fillData[i]  = m_fileContent[m_offset];

			if (++m_offset >= m_fileContent.size())
			{
				m_offset = 0;
			}
		}

		return m_fillData;
	}
}
