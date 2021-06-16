#pragma once

#include <filesystem>

#include "KuuraFillConsumer.hpp"

namespace Kuura
{
	class Drive : public IFillConsumer
	{
	public:
		explicit Drive(const std::filesystem::path& path);
		Drive(Drive&&) noexcept;
		Drive& operator = (Drive&&) noexcept;
		Drive(const Drive&) = delete;
		Drive& operator = (const Drive&) = delete;
		~Drive();

		bool IsValid() const;
		bool Unmount() const;
		std::optional<uint64_t> Size() const override;
		std::optional<uint64_t> OptimalWriteSize() const override;
		std::pair<bool, uint64_t> Write(const std::span<std::byte> data) override;
		std::pair<bool, std::vector<std::byte>> Read(uint64_t bytes, uint64_t offset) override;

	private:
		std::optional<uint64_t> _actualSize;
		std::optional<uint64_t> _optimalWriteSize;

#if defined(_WIN32)
		void* _handle = reinterpret_cast<void*>(-1);
#else
		int _descriptor = -1;
#endif
	};
}
