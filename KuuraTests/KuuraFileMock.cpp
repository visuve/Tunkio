#include "KuuraTests-PCH.hpp"
#include "FillConsumers/KuuraFile.hpp"

namespace Kuura
{
	File::File(const std::filesystem::path& path) :
		IFillConsumer(path),
		m_allocationSize(2048),
		m_alignmentSize(512),
		m_optimalWriteSize(1024)
	{
	}

	File::File(File&& other) noexcept :
		IFillConsumer("")
	{
		*this = std::move(other);
	}

	File& File::operator = (File&& other) noexcept
	{
		if (this != &other)
		{
#if defined(_WIN32)
			std::swap(m_handle, other.m_handle);
#else
			std::swap(m_descriptor, other.m_descriptor);
#endif
			std::swap(m_allocationSize, other.m_allocationSize);
			std::swap(m_alignmentSize, other.m_alignmentSize);
			std::swap(m_optimalWriteSize, other.m_optimalWriteSize);
		}

		return *this;
	}

	File::~File()
	{
	}

	bool File::IsValid() const
	{
		return true;
	}

	std::optional<uint64_t> File::Size() const
	{
		return m_allocationSize;
	}

	std::optional<uint64_t> File::AlignmentSize() const
	{
		return m_alignmentSize;
	}

	std::optional<uint64_t> File::OptimalWriteSize() const
	{
		return m_optimalWriteSize;
	}

	std::pair<bool, uint64_t> File::Write(const std::span<std::byte> data)
	{
		return { !data.empty(), data.size() };
	}

	std::pair<bool, std::vector<std::byte>> File::Read(uint64_t size, uint64_t)
	{
		return { size > 0, std::vector<std::byte>(1, std::byte(0xFF)) };
	}

	bool File::Flush()
	{
		return true;
	}

	bool File::Delete()
	{
		return true;
	}
}
