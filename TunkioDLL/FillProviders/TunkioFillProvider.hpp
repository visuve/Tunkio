#pragma once

#include "TunkioDataUnits.hpp"

#if defined(_WIN32)
#undef min
#undef max
#endif

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

		virtual const void* Data() = 0;
		virtual uint64_t Size(uint64_t bytesLeft) const
		{
			return std::min(bytesLeft, m_size.Bytes());
		}

	protected:
		DataUnit::Bytes m_size;
		bool m_verify;
	};
}
