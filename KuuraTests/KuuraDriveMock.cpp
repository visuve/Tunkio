#include "KuuraTests-PCH.hpp"
#include "FillConsumers/KuuraDrive.hpp"

namespace Kuura
{
	Drive::Drive(const std::filesystem::path& path) :
		IFillConsumer(path),
		_actualSize(2048),
		_optimalWriteSize(1024)
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
			std::swap(Path, other.Path);
#if defined(_WIN32)
			std::swap(_handle, other._handle);
#else
			std::swap(_descriptor, other._descriptor);
#endif
			std::swap(_actualSize, other._actualSize);
			std::swap(_optimalWriteSize, other._optimalWriteSize);
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
		return _actualSize;
	}

	std::optional<uint64_t> Drive::OptimalWriteSize() const
	{
		return _optimalWriteSize;
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
