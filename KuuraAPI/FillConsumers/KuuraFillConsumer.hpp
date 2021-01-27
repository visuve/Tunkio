#pragma once

#include <vector>
#include <span>
#include <utility>

namespace Kuura
{
	class IFillConsumer
	{
	public:
		IFillConsumer() = default;
		virtual ~IFillConsumer() = default;

		virtual std::pair<bool, uint64_t> Size() const = 0;
		virtual std::pair<bool, uint64_t> AlignmentSize() const = 0;
		virtual std::pair<bool, uint64_t> OptimalWriteSize() const = 0;

		virtual std::pair<bool, uint64_t> Write(const std::span<std::byte> data) = 0;
		virtual std::pair<bool, std::vector<std::byte>> Read(uint64_t bytes, uint64_t offset) = 0;
	};
}
