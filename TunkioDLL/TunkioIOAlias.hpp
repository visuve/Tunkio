#pragma once

#ifndef TESTING
#include <Windows.h>
#include <filesystem>

namespace Tunkio::IO
{
    namespace FileSystem = std::filesystem;
    using Path = std::filesystem::path;
    using DirectoryIterator = std::filesystem::recursive_directory_iterator;
    using FileStream = std::ofstream;
    using StreamBuffer = std::streambuf;

    constexpr auto Win32Open = CreateFileW;
    constexpr auto Win32DeviceIoControl = DeviceIoControl;
    constexpr auto Win32Write = WriteFile;
}
#else
#include "../TunkioTests/TunkioIOMock.hpp"

namespace Tunkio::IO
{
    using FileSystem = FileSystemMock;
    using Path = PathMock;
    using FileStream = FileStreamMock;
    using StreamBuffer = StreamBufferMock;
    // using DirectoryIterator = Tunkio::IO::DirectoryIteratorMock;

    constexpr auto Win32Open = Win32OpenMock;
    constexpr auto Win32DeviceIoControl = Win32DeviceIoControlMock;
    constexpr auto Win32Write = Win32WriteMock;
    }
#endif