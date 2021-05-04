#include "KuuraTests-PCH.hpp"
#include "FillConsumers/KuuraFile.hpp"

namespace Kuura
{
	File::File(const std::filesystem::path& path) :
		IFillConsumer(path),
		_allocationSize(2048),
		_optimalWriteSize(1024)
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
			std::swap(Path, other.Path);
#if defined(_WIN32)
			std::swap(_handle, other._handle);
#else
			std::swap(_descriptor, other._descriptor);
#endif
			std::swap(_allocationSize, other._allocationSize);
			std::swap(_optimalWriteSize, other._optimalWriteSize);
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
		return _allocationSize;
	}

	std::optional<uint64_t> File::AlignmentSize() const
	{
		return 0;
	}

	std::optional<uint64_t> File::OptimalWriteSize() const
	{
		return _optimalWriteSize;
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
