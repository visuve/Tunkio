#include "KuuraTests-PCH.hpp"
#include "FillConsumers/KuuraDrive.hpp"

namespace Kuura
{
	Drive::Drive(const std::filesystem::path& path) :
		IFillConsumer(path),
		m_actualSize(2048),
		m_alignmentSize(512),
		m_optimalWriteSize(1024)
	{
	}

	Drive::Drive(Drive&& other) noexcept :
		IFillConsumer("")
	{
		*this = std::move(other);
	}

	Drive& Drive::operator = (Drive&& other) noexcept
	{
		if (this != &other)
		{
#if defined(_WIN32)
			std::swap(m_handle, other.m_handle);
#else
			std::swap(m_descriptor, other.m_descriptor);
#endif
			std::swap(m_actualSize, other.m_actualSize);
			std::swap(m_alignmentSize, other.m_alignmentSize);
			std::swap(m_optimalWriteSize, other.m_optimalWriteSize);
		}

		return *this;
	}

	Drive::~Drive()
	{
	}

	bool Drive::IsValid() const
	{
		return true;
	}

	std::optional<uint64_t> Drive::Size() const
	{
		return m_actualSize;
	}

	std::optional<uint64_t> Drive::AlignmentSize() const
	{
		return m_alignmentSize;
	}

	std::optional<uint64_t> Drive::OptimalWriteSize() const
	{
		return m_optimalWriteSize;
	}

	std::pair<bool, uint64_t> Drive::Write(const std::span<std::byte> data)
	{
		return { !data.empty(), data.size() };
	}

	std::pair<bool, std::vector<std::byte>> Drive::Read(uint64_t size, uint64_t)
	{
		return { size > 0, std::vector<std::byte>(1, std::byte(0xFF)) };
	}

	bool Drive::Unmount() const
	{
		return true;
	}
}
