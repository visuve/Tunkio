#pragma once

#include "AutoHandle.hpp"
#include <functional>

namespace WDW
{
    using ProgressCallback = std::function<void(uint64_t bytesWritten, uint64_t secondsElapsed)>;

    uint64_t DiskSize(const AutoHandle& hdd);
    bool Wipe(const AutoHandle& handle, uint64_t& bytesLeft, uint64_t& writtenBytesTotal, const ProgressCallback& progress) ;
}