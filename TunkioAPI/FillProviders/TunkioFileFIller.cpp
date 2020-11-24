#include "../TunkioAPI-PCH.hpp"
#include "TunkioFileFiller.hpp"
#include <fstream>

namespace Tunkio
{
	std::vector<uint8_t> ReadContents(const std::filesystem::path& path)
	{
		// TODO: with big files it would be nice to have some kind of progress
		std::basic_ifstream<uint8_t> stream(path, std::ios_base::binary);
		return { std::istreambuf_iterator<uint8_t>(stream), std::istreambuf_iterator<uint8_t>() };
	}

	FileFiller::FileFiller(DataUnit::Bytes bytes, const std::filesystem::path& path, bool verify) :
		IFillProvider(bytes, verify),
		m_fileContent(ReadContents(path))
	{
	}

	bool FileFiller::HasContent()
	{
		return m_fileContent.size() > 0;
	}

	const void* FileFiller::Data()
	{
		if (m_fillData.empty())
		{
			while (m_fillData.size() < m_size.Bytes())
			{
				m_fillData.insert(m_fillData.end(), m_fileContent.begin(), m_fileContent.end());

				if (m_fillData.size() < m_size.Bytes())
				{
					m_fillData.push_back('\0');
				}
			}
		}

		return m_fillData.data();
	}
}