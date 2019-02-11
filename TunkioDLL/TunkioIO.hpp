#pragma once

#include "TunkioAPI.h"
#include "TunkioHandle.hpp"

#include <functional>

#ifdef TESTING
#include "../TunkioTests/TunkioIOMock.hpp"
#else
#include <filesystem>
#endif

namespace Tunkio::IO
{

#ifdef TESTING
    using FileSystem = FileSystemMock;
    using Path = PathMock;
    using FileStream = FileStreamMock;
    using StreamBuffer = StreamBufferMock;

    constexpr auto Win32Open = Win32OpenMock;
    constexpr auto Win32DeviceIoControl = Win32DeviceIoControlMock;
    constexpr auto Win32Write = Win32WriteMock;
#else
    namespace FileSystem = std::filesystem;
    using Path = std::filesystem::path;
    using FileStream = std::ofstream;
    using StreamBuffer = std::streambuf;

    constexpr auto Win32Open = CreateFileW;
    constexpr auto Win32DeviceIoControl = DeviceIoControl;
    constexpr auto Win32Write = WriteFile;
#endif

    namespace File
    {
        FileStream Open(const Path& file);
        uint64_t Size(const Path& file);
        bool Fill(FileStream& file, uint64_t& bytesLeft, uint64_t& writtenBytesTotal, TunkioProgressCallback progress);
        bool Remove(const Path& path);
    }

    namespace Directory
    {
        // TODO!
    }

    namespace Volume
    {
        RawHandle Open(const std::wstring& path);
        uint64_t Size(const AutoHandle& volume);
        bool Fill(const AutoHandle& handle, uint64_t& bytesLeft, uint64_t& writtenBytesTotal, TunkioProgressCallback progress);
    }
}