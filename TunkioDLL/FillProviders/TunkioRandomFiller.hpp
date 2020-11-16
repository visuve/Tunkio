#pragma once

#include "TunkioCharFiller.hpp"

namespace Tunkio
{
	class RandomFiller : public CharFiller
	{
	public:
		explicit RandomFiller(DataUnit::Bytes bytes, bool verify);
		~RandomFiller();

		uint8_t* Data() override;
	};
}