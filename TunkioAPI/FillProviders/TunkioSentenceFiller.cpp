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

			for (size_t i = 0; i < bytes; ++i)
			{
				size_t mod = i % m_fillString.size();
				m_fillData[i] = m_fillString[mod];
			}
		}

		return m_fillData.data();
	}
}
