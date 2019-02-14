#pragma once

#include "TunkioAPI.h"

#include <functional>

#ifndef TESTING
#include <fstream>
using FileStream = std::ofstream;
using StreamBuffer = std::streambuf;
#else
#include "TunkioOutputMock.hpp"
using FileStream = Tunkio::FileStreamMock;
using StreamBuffer = Tunkio::StreamBufferMock;
#endif

namespace Tunkio::Output
{
    bool Fill(FileStream& file, uint64_t& bytesLeft, uint64_t& writtenBytesTotal, TunkioProgressCallback progress);
}