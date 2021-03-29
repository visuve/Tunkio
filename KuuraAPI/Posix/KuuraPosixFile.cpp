#include "../KuuraAPI-PCH.hpp"
#include "../FillConsumers/KuuraFile.hpp"
#include "KuuraPosixIO.hpp"

namespace Kuura
{
	// https://linux.die.net/man/2/open
	constexpr uint32_t OpenFlags = O_RDWR | O_DIRECT | O_SYNC;

	File::File(const std::filesystem::path& path) :
		IFillConsumer(path),
		m_descriptor(open(path.c_str(), OpenFlags))
	{
		if (!IsValid())
		{
			return;
		}

		std::optional<FileInfo> fileInfo =
			FileInfoFromDescriptor(m_descriptor);

		if (!fileInfo)
		{
			return;
		}

		m_allocationSize = fileInfo->st_blocks * 512;

		// See notes in KuuraWin32File.cpp

		m_alignmentSize = fileInfo->st_blksize;
		m_optimalWriteSize = fileInfo->st_blksize / 512 * 0x10000;

		if (m_alignmentSize.value() % 512 != 0)
		{
			// Something is horribly wrong
			m_allocationSize = std::nullopt;
			m_alignmentSize = std::nullopt;
			m_optimalWriteSize = std::nullopt;
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
			std::swap(m_descriptor, other.m_descriptor);
			std::swap(m_allocationSize, other.m_allocationSize);
			std::swap(m_alignmentSize, other.m_alignmentSize);
			std::swap(m_optimalWriteSize, other.m_optimalWriteSize);
		}

		return *this;
	}

	File::~File()
	{
		if (m_descriptor)
		{
			close(m_descriptor);
			m_descriptor = -1;
		}
	}

	bool File::IsValid() const
	{
		return m_descriptor > 0;
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
		return WriteTo(m_descriptor, data);
	}

	std::pair<bool, std::vector<std::byte>> File::Read(uint64_t bytes, uint64_t offset)
	{
		return ReadFrom(m_descriptor, bytes, offset);
	}

	bool File::Flush()
	{
		return fsync(m_descriptor) == 0;
	}

	bool File::Delete()
	{
		if (IsValid() && close(m_descriptor))
		{
			m_descriptor = -1;
			return remove(Path.c_str()) == 0;
		}

		return false;
	}
}
