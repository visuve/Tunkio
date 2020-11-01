#pragma once

#include "TunkioCharFiller.hpp"

namespace Tunkio
{
	class RandomFiller : public CharFiller
	{
	public:
		explicit RandomFiller(DataUnit::Byte bytes = DataUnit::Mebibyte(1));
		~RandomFiller();

		uint8_t* Data() override;

	private:
		DataUnit::Byte m_size;
		std::vector<uint8_t> m_fillData;
	};
}