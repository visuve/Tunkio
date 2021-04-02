#include "../KuuraAPI-PCH.hpp"
#include "../FillConsumers/KuuraFile.hpp"
#include "KuuraWin32IO.hpp"

namespace Kuura
{
	File::File(const std::filesystem::path& path) :
		IFillConsumer(path),
		_handle(Open(path))
	{
		if (!IsValid())
		{
			return;
		}

		_allocationSize = AllocationSizeByHandle(_handle);
		_optimalWriteSize = OptimalWriteSizeByHandle(_handle);
		_alignmentSize = PageSize();

		if (!_allocationSize || _allocationSize.value() % 512 != 0)
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
			std::swap(_handle, other._handle);
			std::swap(_allocationSize, other._allocationSize);
			std::swap(_alignmentSize, other._alignmentSize);
			std::swap(_optimalWriteSize, other._optimalWriteSize);
		}

		return *this;
	}

	File::~File()
	{
		if (IsValid())
		{
			CloseHandle(_handle);
			_handle = nullptr;
		}
	}

	bool File::IsValid() const
	{
		return _handle != nullptr && _handle != INVALID_HANDLE_VALUE;
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
		DWORD bytesWritten = 0;
		DWORD bytesToWrite = static_cast<DWORD>(data.size_bytes());

		if (!WriteFile(_handle, data.data(), bytesToWrite, &bytesWritten, nullptr))
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

		if (!ReadFile(_handle, buffer.data(), static_cast<DWORD>(bytes), &bytesRead, &overlapped))
		{
			return { false, {} };
		}

		return { bytes == bytesRead, std::move(buffer) };
	}

	bool File::Flush()
	{
		return FlushFileBuffers(_handle);
	}

	bool File::Delete()
	{
		if (IsValid() && CloseHandle(_handle))
		{
			_handle = nullptr;
			return DeleteFileW(Path.c_str());
		}

		return false;
	}
};
