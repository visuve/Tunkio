#pragma once

#include "KuuraFillProvider.hpp"

namespace Kuura
{
	class RandomFiller : public IFillProvider
	{
	public:
		explicit RandomFiller(bool verify);
		~RandomFiller() = default;

		std::span<std::byte> Data(uint64_t bytes) override;
	};
}
