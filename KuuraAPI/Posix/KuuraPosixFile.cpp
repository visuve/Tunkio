#include "../KuuraAPI-PCH.hpp"
#include "../FillConsumers/KuuraFile.hpp"
#include "KuuraPosixIO.hpp"

namespace Kuura
{
	// https://linux.die.net/man/2/open
	constexpr uint32_t OpenFlags = O_RDWR | O_DIRECT | O_SYNC;

	File::File(const std::filesystem::path& path) :
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

		_allocationSize = fileInfo->st_blocks * 512;

		// See notes in KuuraWin32File.cpp

		_alignmentSize = fileInfo->st_blksize;
		_optimalWriteSize = fileInfo->st_blksize / 512 * 0x10000;

		if (_alignmentSize.value() % 512 != 0)
		{
			// Something is horribly wrong
			_allocationSize = std::nullopt;
			_alignmentSize = std::nullopt;
			_optimalWriteSize = std::nullopt;
		}
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
			std::swap(_descriptor, other._descriptor);
			std::swap(_allocationSize, other._allocationSize);
			std::swap(_alignmentSize, other._alignmentSize);
			std::swap(_optimalWriteSize, other._optimalWriteSize);
		}

		return *this;
	}

	File::~File()
	{
		if (_descriptor)
		{
			close(_descriptor);
			_descriptor = -1;
		}
	}

	bool File::IsValid() const
	{
		return _descriptor > 0;
	}

	std::optional<uint64_t> File::Size() const
	{
		return _allocationSize;
	}

	std::optional<uint64_t> File::AlignmentSize() const
	{
		return _alignmentSize;
	}

	std::optional<uint64_t> File::OptimalWriteSize() const
	{
		return _optimalWriteSize;
	}

	std::pair<bool, uint64_t> File::Write(const std::span<std::byte> data)
	{
		return WriteTo(_descriptor, data);
	}

	std::pair<bool, std::vector<std::byte>> File::Read(uint64_t bytes, uint64_t offset)
	{
		return ReadFrom(_descriptor, bytes, offset);
	}

	bool File::Flush()
	{
		return fsync(_descriptor) == 0;
	}

	bool File::Delete()
	{
		if (IsValid() && close(_descriptor))
		{
			_descriptor = -1;
			return remove(Path.c_str()) == 0;
		}

		return false;
	}
}
