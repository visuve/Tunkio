#pragma once

#include "TunkioFillProvider.hpp"

namespace Tunkio
{
	class CharFiller : public IFillProvider
	{
	public:
		explicit CharFiller(char8_t byte, bool verify);
		virtual ~CharFiller();

		const void* Data(uint64_t bytes) override;

	protected:
		char8_t m_fillChar;
		std::vector<char8_t> m_fillData;
	};
}
