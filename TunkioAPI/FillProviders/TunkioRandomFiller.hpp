#pragma once

#include "TunkioFillProvider.hpp"

namespace Tunkio
{
	class RandomFiller : public IFillProvider
	{
	public:
		explicit RandomFiller(bool verify);
		~RandomFiller() = default;

		std::byte* Data(uint64_t bytes) override;
	};
}
