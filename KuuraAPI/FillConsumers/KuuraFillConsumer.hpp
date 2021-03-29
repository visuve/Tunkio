#pragma once

#include <filesystem>
#include <optional>
#include <span>
#include <utility>
#include <vector>

namespace Kuura
{
	class IFillConsumer
	{
	public:
		inline explicit IFillConsumer(const std::filesystem::path& path) :
			Path(path)
		{
		}

		virtual ~IFillConsumer() = default;

		virtual std::optional<uint64_t> Size() const = 0;
		virtual std::optional<uint64_t> AlignmentSize() const = 0;
		virtual std::optional<uint64_t> OptimalWriteSize() const = 0;

		virtual std::pair<bool, uint64_t> Write(const std::span<std::byte> data) = 0;
		virtual std::pair<bool, std::vector<std::byte>> Read(uint64_t bytes, uint64_t offset) = 0;

	protected:
		std::filesystem::path Path;
	};
}
