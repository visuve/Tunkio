#include "../TunkioAPI-PCH.hpp"
#include "TunkioStringFiller.hpp"

namespace Tunkio
{
	StringFiller::StringFiller(
		DataUnit::Bytes bytes,
		const std::string& fillString,
		bool verify) :
		IFillProvider(bytes, verify),
		m_fillString(fillString)
	{
	}

	StringFiller::~StringFiller()
	{
	}

	uint8_t* StringFiller::Data()
	{
		if (m_fillData.empty())
		{
			while (m_fillData.size() < m_size.Bytes())
			{
				m_fillData.append(m_fillString);
				m_fillData.append(1, '\0');
			}

			m_fillString.clear(); // Not needed anymore
		}

		return reinterpret_cast<uint8_t*>(m_fillData.data());
	}

	uint64_t StringFiller::Size(uint64_t bytesLeft)
	{
		if (m_fillData.size() > bytesLeft)
		{
			return bytesLeft;
		}

		return m_fillData.size();
	}
}
