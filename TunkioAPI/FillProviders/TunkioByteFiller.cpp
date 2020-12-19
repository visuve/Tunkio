#include "../TunkioAPI-PCH.hpp"
#include "TunkioByteFiller.hpp"

namespace Tunkio
{
	ByteFiller::ByteFiller(std::byte byte, bool verify) :
		IFillProvider(verify),
		m_byte(byte)
	{
	}

	std::span<std::byte> ByteFiller::Data(uint64_t bytes, uint64_t alignment)
	{
		if (m_fillData.size() != bytes)
		{
			m_fillData.resize(bytes, m_byte);
			AlignData(alignment);
		}

		return m_fillData;
	}
}
