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

	SentenceFiller::~SentenceFiller()
	{
	}

	const void* SentenceFiller::Data(uint64_t bytes)
	{
		if (m_fillData.size() != bytes)
		{
			m_fillData.resize(bytes);
		}

		for (char& c : m_fillData)
		{
			c = m_fillString[m_offset++];

			if (m_offset >= m_fillString.size())
			{
				m_offset = 0;
			}
		}

		return m_fillData.data();
	}
}
