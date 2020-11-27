#include "../TunkioAPI-PCH.hpp"
#include "TunkioCharFiller.hpp"

namespace Tunkio
{
	CharFiller::CharFiller(char8_t byte, bool verify) :
		IFillProvider(verify),
		m_fillChar(byte)
	{
	}

	CharFiller::~CharFiller()
	{
		m_fillData.clear();
	}

	const void* CharFiller::Data(uint64_t bytes)
	{
		if (m_fillData.size() != bytes)
		{
			m_fillData.resize(bytes, m_fillChar);
		}

		return m_fillData.data();
	}
}
