#pragma once

#include <filesystem>

#include "KuuraFillConsumer.hpp"

namespace Kuura
{
	class File : public IFillConsumer
	{
	public:
		const std::filesystem::path Path;

		explicit File(const std::filesystem::path& path);
		File(File&&) noexcept;
		File& operator = (File&&) noexcept;
		File(const File&) = delete;
		File& operator = (const File&) = delete;
		~File();

		bool IsValid() const;
		std::pair<bool, uint64_t> Size() const override;
		std::pair<bool, uint64_t> AlignmentSize() const override;
		std::pair<bool, uint64_t> OptimalWriteSize() const override;
		std::pair<bool, uint64_t> Write(const std::span<std::byte> data) override;
		std::pair<bool, std::vector<std::byte>> Read(uint64_t bytes, uint64_t offset) override;
		bool Flush();
		bool Delete();

	private:
		std::pair<bool, uint64_t> m_allocationSize;
		std::pair<bool, uint64_t> m_alignmentSize;
		std::pair<bool, uint64_t> m_optimalWriteSize;

#if defined(_WIN32)
		void* m_handle = reinterpret_cast<void*>(-1);
#else
		int m_fileDescriptor = -1;
#endif
	};
}
