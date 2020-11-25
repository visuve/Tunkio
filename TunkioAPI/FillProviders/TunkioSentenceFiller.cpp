#include "../TunkioAPI-PCH.hpp"
#include "TunkioSentenceFiller.hpp"

namespace Tunkio
{
	SentenceFiller::SentenceFiller(
		DataUnit::Bytes bytes,
		const std::string& fillString,
		bool verify) :
		IFillProvider(bytes, verify),
		m_fillString(fillString)
	{
	}

	SentenceFiller::~SentenceFiller()
	{
	}

	const void* SentenceFiller::Data()
	{
		if (m_fillData.empty())
		{
			while (m_fillData.size() < m_size.Bytes())
			{
				m_fillData.append(m_fillString);
			}
		}

		return m_fillData.data();
	}
}
