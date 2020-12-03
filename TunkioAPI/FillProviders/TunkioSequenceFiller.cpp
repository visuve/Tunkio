#include "../TunkioAPI-PCH.hpp"
#include "TunkioSequenceFiller.hpp"

namespace Tunkio
{
	SequenceFiller::SequenceFiller(
		const std::string& fillString,
		bool verify) :
		IFillProvider(verify),
		m_fillString(fillString)
	{
	}

	std::span<std::byte> SequenceFiller::Data(uint64_t bytes, uint64_t alignment)
	{
		if (m_fillData.size() != bytes)
		{
			m_fillData.resize(bytes);
			AlignData(bytes, alignment);
		}

		for (size_t i = 0; i < bytes; ++i)
		{
			m_fillData[i] = static_cast<std::byte>(m_fillString[m_offset++]);

			if (m_offset >= m_fillString.size())
			{
				m_offset = 0;
			}
		}

		return m_fillData;
	}
}
