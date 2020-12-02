#include "../TunkioAPI-PCH.hpp"
#include "TunkioByteFiller.hpp"

namespace Tunkio
{
	ByteFiller::ByteFiller(std::byte byte, bool verify) :
		IFillProvider(verify),
		m_byte(byte)
	{
	}

	std::byte* ByteFiller::Data(uint64_t bytes)
	{
		if (m_fillData.size() != bytes)
		{
			m_fillData.resize(bytes, m_byte);
		}

		return m_fillData.data();
	}
}
