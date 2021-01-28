#pragma once

#include <filesystem>

#include "KuuraFillConsumer.hpp"

namespace Kuura
{
	class Drive : public IFillConsumer
	{
	public:
		const std::filesystem::path Path;

		explicit Drive(const std::filesystem::path& path);
		Drive(Drive&&) noexcept;
		Drive& operator = (Drive&&) noexcept;
		Drive(const Drive&) = delete;
		Drive& operator = (const Drive&) = delete;
		~Drive();

		bool IsValid() const;
		bool Unmount() const;
		std::optional<uint64_t> Size() const override;
		std::optional<uint64_t> AlignmentSize() const override;
		std::optional<uint64_t> OptimalWriteSize() const override;
		std::pair<bool, uint64_t> Write(const std::span<std::byte> data) override;
		std::pair<bool, std::vector<std::byte>> Read(uint64_t bytes, uint64_t offset) override;

	private:
		std::optional<uint64_t> m_actualSize;
		std::optional<uint64_t> m_alignmentSize;
		std::optional<uint64_t> m_optimalWriteSize;

#if defined(_WIN32)
		void* m_handle = reinterpret_cast<void*>(-1);
#else
		int m_descriptor = -1;
#endif
	};
}
