#pragma once

#ifndef TESTING
#include <filesystem>
namespace FileSystem = std::filesystem;
using Path = std::filesystem::path;
#else
#include "TunkioFileSystemMock.hpp"
namespace FileSystem = Tunkio::FileSystemMock;
using Path = Tunkio::FileSystemMock::PathMock;
#endif