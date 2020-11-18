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

	const void* StringFiller::Data()
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

		return m_fillData.data();
	}
}
