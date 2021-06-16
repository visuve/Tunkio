#pragma once

#include "KuuraFillProvider.hpp"

namespace Kuura
{
	class ByteFiller : public IFillProvider
	{
	public:
		explicit ByteFiller(std::byte byte, bool verify);
		~ByteFiller() = default;

		std::span<std::byte> Data(uint64_t bytes) override;

	protected:
		std::byte _byte;
	};
}
