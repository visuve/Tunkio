#pragma once

#include "TunkioFillProvider.hpp"

namespace Tunkio
{
	class CharFiller : public IFillProvider
	{
	public:
		explicit CharFiller(uint8_t byte, DataUnit::Bytes bytes = DataUnit::Mebibytes(1));
		virtual ~CharFiller();

		uint8_t* Data() override;
		uint64_t Size(uint64_t bytesLeft) override;

	protected:
		uint8_t m_fillChar;
		std::vector<uint8_t> m_fillData;
	};
}