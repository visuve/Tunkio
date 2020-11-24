#include "../TunkioAPI-PCH.hpp"
#include "TunkioSentenceFiller.hpp"

namespace Tunkio
{
	char FindPaddingCharacter(std::string& sentence)
	{
		if (sentence.length() > 2)
		{
			std::string tail = sentence.substr(sentence.size() - 2, sentence.size());

			if (tail == "\\n")
			{
				sentence.erase(sentence.size() - 2, 2);
				return '\n';
			}
		}

		return '\0';
	}

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
			const char padding = FindPaddingCharacter(m_fillString);

			while (m_fillData.size() < m_size.Bytes())
			{
				m_fillData.append(m_fillString);

				if (m_fillData.size() < m_size.Bytes())
				{
					m_fillData.append(1, padding);
				}
			}
		}

		return m_fillData.data();
	}
}
