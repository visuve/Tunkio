#pragma once

#include "TunkioFillProvider.hpp"

namespace Tunkio
{
	class StringFiller : public IFillProvider
	{
	public:
		explicit StringFiller(const std::string& fillString, DataUnit::Byte bytes = DataUnit::Mebibyte(1));
		~StringFiller();

		uint8_t* Data() override;
		uint64_t Size(uint64_t bytesLeft) override;

	private:
		std::string m_fillString;
		std::string m_fillData;
	};
}