#pragma once

#include "TunkioDataUnits.hpp"

namespace Tunkio
{
	class IFillProvider
	{
	public:
		constexpr explicit IFillProvider(DataUnit::Bytes bytes, bool verify) :
			m_size(bytes),
			m_verify(verify)
		{
		}

		virtual ~IFillProvider() = default;

		virtual uint8_t* Data() = 0;
		virtual uint64_t Size(uint64_t bytesLeft) = 0;

	protected:
		DataUnit::Bytes m_size;
		bool m_verify;
	};
}