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
			// TODO: this needs to be queried from the underlying volume / partition
			constexpr double SectorSize = 512.00;
			const uint64_t required = std::min(bytesLeft, m_size.Bytes());
			const double factor = required / SectorSize;
			const double rounded = std::ceil(factor);
			return static_cast<uint64_t>(rounded * SectorSize);
		}

	protected:
		DataUnit::Bytes m_size;
		bool m_verify;
	};
}
