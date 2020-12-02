#include "../TunkioAPI-PCH.hpp"
#include "TunkioSentenceFiller.hpp"

namespace Tunkio
{
	SentenceFiller::SentenceFiller(
		const std::string& fillString,
		bool verify) :
		IFillProvider(verify),
		m_fillString(fillString)
	{
	}

	std::byte* SentenceFiller::Data(uint64_t bytes)
	{
		if (m_fillData.size() != bytes)
		{
			m_fillData.resize(bytes);
		}

		for (std::byte& byte : m_fillData)
		{
			byte = std::byte(m_fillString[m_offset++]);

			if (m_offset >= m_fillString.size())
			{
				m_offset = 0;
			}
		}

		return m_fillData.data();
	}
}
