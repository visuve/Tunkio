#pragma once

#include "TunkioHandle.hpp"

#include <functional>

namespace Tunkio::IO
{
    using ProgressCallback = std::function<void(uint64_t bytesWritten, uint64_t secondsElapsed)>;

    RawHandle OpenW(const std::wstring& path);
    RawHandle OpenA(const std::string& path);

    uint64_t DiskSize(const AutoHandle& hdd);
    bool Wipe(const AutoHandle& handle, uint64_t& bytesLeft, uint64_t& writtenBytesTotal, const ProgressCallback& progress);
}