#pragma once

#include "TunkioHandle.hpp"

#include <functional>

namespace Tunkio::IO
{
    RawHandle Open(const std::wstring& path);
    uint64_t FileSize(const AutoHandle& file);
    uint64_t VolumeSize(const AutoHandle& volume);

    using ProgressCallback = std::function<void(uint64_t bytesWritten, uint64_t secondsElapsed)>;
    bool Wipe(const AutoHandle& handle, uint64_t& bytesLeft, uint64_t& writtenBytesTotal, const ProgressCallback& progress);
}