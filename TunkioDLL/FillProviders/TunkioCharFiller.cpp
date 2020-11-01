#include "../PCH.hpp"
#include "TunkioCharFiller.hpp"

namespace Tunkio
{
	CharFiller::CharFiller(uint8_t byte, DataUnit::Byte bytes) :
		IFillProvider(bytes),
		m_fillChar(byte)
	{
	}

	CharFiller::~CharFiller()
	{
		m_fillData.clear();
	}

	uint8_t* CharFiller::Data()
	{
		if (m_fillData.empty())
		{
			m_fillData.resize(m_size.Bytes(), m_fillChar);
		}

		return m_fillData.data();
	}

	uint64_t CharFiller::Size(uint64_t bytesLeft)
	{
		if (m_fillData.size() > bytesLeft)
		{
			return bytesLeft;
		}

		return m_fillData.size();
	}
}