#pragma once

#include <filesystem>
#include <span>

#include "KuuraFillConsumer.hpp"

namespace Kuura
{
	class File : public IFillConsumer
	{
	public:
		explicit File(const std::filesystem::path& path);
		File(File&&) noexcept;
		File& operator = (File&&) noexcept;
		File(const File&) = delete;
		File& operator = (const File&) = delete;
		~File();

		bool IsValid() const;
		std::optional<uint64_t> Size() const override;
		std::optional<uint64_t> OptimalWriteSize() const override;
		std::pair<bool, uint64_t> Write(const std::span<std::byte> data) override;
		std::pair<bool, std::vector<std::byte>> Read(uint64_t bytes, uint64_t offset) override;
		bool Flush();
		bool Delete();

	private:
		std::optional<uint64_t> _allocationSize;
		std::optional<uint64_t> _optimalWriteSize;

#if defined(_WIN32)
		void* _handle = reinterpret_cast<void*>(-1);
#else
		int _descriptor = -1;
#endif
	};
}
