#include "../PCH.hpp"
#include "TunkioStringFiller.hpp"

namespace Tunkio
{
	StringFiller::StringFiller(const std::string& fillString, DataUnit::Bytes bytes) :
		IFillProvider(bytes),
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
			while (m_fillData.size() < DataUnit::Mebibytes(1).Bytes())
			{
				m_fillData += m_fillString;
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