#include "../TunkioAPI-PCH.hpp"
#include "TunkioSentenceFiller.hpp"

namespace Tunkio
{
	void ReplaceAll(std::string& sentence, const std::string& from, const std::string& to)
	{
		for (size_t position = sentence.find(from, 0);
			position != std::string::npos;
			position = sentence.find(from, position))
		{
			sentence.replace(position, from.length(), to);
			position += to.length();
		}
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
			std::string padding;

			ReplaceAll(m_fillString, "\\f", "\f");
			ReplaceAll(m_fillString, "\\n", "\n");
			ReplaceAll(m_fillString, "\\r", "\r");
			ReplaceAll(m_fillString, "\\t", "\t");
			ReplaceAll(m_fillString, "\\v", "\v");

			const size_t paddingStart = m_fillString.find_first_of("\f\n\r\t\v");

			if (paddingStart != std::string::npos)
			{
				padding = m_fillString.substr(paddingStart, m_fillString.size());
				m_fillString.erase(paddingStart, m_fillString.size());
			}
			else
			{
				padding.append(1, '\0');
			}

			while (m_fillData.size() < m_size.Bytes())
			{
				m_fillData.append(m_fillString);

				if (m_fillData.size() + m_fillString.size() < m_size.Bytes())
					m_fillData.append(padding);
			}
		}

		return m_fillData.data();
	}
}
