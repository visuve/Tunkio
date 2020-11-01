#pragma once

#include "TunkioDataUnits.hpp"

namespace Tunkio
{
	class IFillProvider
	{
	public:
		constexpr explicit IFillProvider(DataUnit::Byte bytes) :
			m_size(bytes)
		{
		}

		virtual ~IFillProvider() = default;

		virtual uint8_t* Data() = 0;
		virtual uint64_t Size(uint64_t bytesLeft) = 0;

	protected:
		DataUnit::Byte m_size;
	};
}