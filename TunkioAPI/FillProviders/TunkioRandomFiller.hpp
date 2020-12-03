#pragma once

#include "TunkioFillProvider.hpp"

namespace Tunkio
{
	class RandomFiller : public IFillProvider
	{
	public:
		explicit RandomFiller(bool verify);
		~RandomFiller() = default;

		std::span<std::byte> Data(uint64_t bytes, uint64_t alignment) override;
	};
}
