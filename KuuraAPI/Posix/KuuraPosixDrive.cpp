#include "../KuuraAPI-PCH.hpp"
#include "../FillConsumers/KuuraDrive.hpp"
#include "KuuraPosixIO.hpp"

namespace Kuura
{
#if defined(__linux__)
	constexpr unsigned long int DiskSizeRequest = BLKGETSIZE64;
#else
	constexpr unsigned long int DiskSizeRequest = DIOCGMEDIASIZE;
#endif

	// https://linux.die.net/man/2/open
	constexpr uint32_t OpenFlags = O_RDWR | O_SYNC;

	std::optional<uint64_t> DiskSizeByDescriptor(const int descriptor)
	{
		uint64_t size = 0;

		if (ioctl(descriptor, DiskSizeRequest, &size) != 0)
		{
			return std::nullopt;
		}

		return size;
	}

	Drive::Drive(const std::filesystem::path& path) :
		IFillConsumer(path),
		_descriptor(open(path.c_str(), OpenFlags))
	{
		if (!IsValid())
		{
			return;
		}

		std::optional<FileInfo> fileInfo =
			FileInfoFromDescriptor(_descriptor);

		if (!fileInfo)
		{
			return;
		}

		_actualSize = DiskSizeByDescriptor(_descriptor);
		_optimalWriteSize = fileInfo->st_blksize / 512 * 0x10000;
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
			std::swap(_descriptor, other._descriptor);
			std::swap(_actualSize, other._actualSize);
			std::swap(_optimalWriteSize, other._optimalWriteSize);
		}

		return *this;
	}

	Drive::~Drive()
	{
		if (_descriptor)
		{
			close(_descriptor);
			_descriptor = -1;
		}
	}

	bool Drive::IsValid() const
	{
		return _descriptor > 0;
	}

	bool Drive::Unmount() const
	{
		// TODO: need to check that it is actually mounted
		// return unmount(Path.c_str(), MNT_FORCE) == 0;
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
		return WriteTo(_descriptor, data);
	}

	std::pair<bool, std::vector<std::byte>> Drive::Read(uint64_t bytes, uint64_t offset)
	{
		return ReadFrom(_descriptor, bytes, offset);
	}
}
