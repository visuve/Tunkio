#include "../TunkioAPI-PCH.hpp"
#include "TunkioCharFiller.hpp"

namespace Tunkio
{
	CharFiller::CharFiller(DataUnit::Bytes bytes, uint8_t byte, bool verify) :
		IFillProvider(bytes, verify),
		m_fillChar(byte)
	{
	}

	CharFiller::~CharFiller()
	{
		m_fillData.clear();
	}

	const void* CharFiller::Data()
	{
		if (m_fillData.empty())
		{
			m_fillData.resize(m_size.Bytes(), m_fillChar);
		}

		return m_fillData.data();
	}
}
