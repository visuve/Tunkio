#pragma once

#include "TunkioFillProvider.hpp"

namespace Tunkio
{
	class CharFiller : public IFillProvider
	{
	public:
		explicit CharFiller(std::byte byte, bool verify);
		~CharFiller() = default;

		std::byte* Data(uint64_t bytes) override;

	protected:
		std::byte m_byte;
	};
}
