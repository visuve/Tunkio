#include "../KuuraAPI-PCH.hpp"
#include "../FillConsumers/KuuraFile.hpp"
#include "KuuraWin32IO.hpp"

namespace Kuura
{
	File::File(const std::filesystem::path& path) :
		Path(path),
		m_handle(Open(path))
	{
		if (!IsValid())
		{
			return;
		}

		m_allocationSize = AllocationSizeByHandle(m_handle);
		m_optimalWriteSize = OptimalWriteSizeByHandle(m_handle);
		m_alignmentSize = PageSize();

		if (!m_allocationSize || m_allocationSize.value() % 512 != 0)
		{
			// Something is horribly wrong
			m_allocationSize = std::nullopt;
			m_alignmentSize = std::nullopt;
			m_optimalWriteSize = std::nullopt;
		}
	}

	File::File(File&& other) noexcept
	{
		std::swap(m_handle, other.m_handle);
		std::swap(m_allocationSize, other.m_allocationSize);
		std::swap(m_alignmentSize, other.m_alignmentSize);
		std::swap(m_optimalWriteSize, other.m_optimalWriteSize);
	}

	File& File::operator = (File&& other) noexcept
	{
		std::swap(m_handle, other.m_handle);
		std::swap(m_allocationSize, other.m_allocationSize);
		std::swap(m_alignmentSize, other.m_alignmentSize);
		std::swap(m_optimalWriteSize, other.m_optimalWriteSize);
		return *this;
	}

	File::~File()
	{
		if (IsValid())
		{
			CloseHandle(m_handle);
			m_handle = nullptr;
		}
	}

	bool File::IsValid() const
	{
		return m_handle != nullptr && m_handle != INVALID_HANDLE_VALUE;
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
		DWORD bytesWritten = 0;
		DWORD bytesToWrite = static_cast<DWORD>(data.size_bytes());

		if (!WriteFile(m_handle, data.data(), bytesToWrite, &bytesWritten, nullptr))
		{
			return { false, bytesWritten };
		}

		return { bytesToWrite == bytesWritten, bytesWritten };
	}

	std::pair<bool, std::vector<std::byte>> File::Read(uint64_t bytes, uint64_t offset)
	{
		std::vector<std::byte> buffer(bytes);
		DWORD bytesRead = 0;
		OVERLAPPED overlapped = Offset(offset);

		if (!ReadFile(m_handle, buffer.data(), static_cast<DWORD>(bytes), &bytesRead, &overlapped))
		{
			return { false, {} };
		}

		return { bytes == bytesRead, std::move(buffer) };
	}

	bool File::Flush()
	{
		return FlushFileBuffers(m_handle);
	}

	bool File::Delete()
	{
		if (IsValid() && CloseHandle(m_handle))
		{
			m_handle = nullptr;
			return DeleteFileW(Path.c_str());
		}

		return false;
	}
};
