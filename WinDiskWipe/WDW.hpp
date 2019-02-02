#pragma once

#include "Units.hpp"

#include <functional>

namespace WDW
{
    using ProgressCallback = std::function<void(uint64_t bytesWritten, uint64_t secondsElapsed)>;

    class AutoHandle
    {
        using RawHandle = void*;
    public:
        AutoHandle(const RawHandle handle);
        AutoHandle(const AutoHandle&) = delete;
        AutoHandle(AutoHandle&&) = delete;
        AutoHandle& operator = (const AutoHandle&) = delete;
        AutoHandle& operator = (AutoHandle&&) = delete;
        ~AutoHandle();

        bool IsValid() const;
        operator const RawHandle() const;
    private:
        RawHandle m_handle = INVALID_HANDLE_VALUE;
    };

    uint64_t DiskSize(const AutoHandle& hdd);
    bool WipeDrive(const AutoHandle& hdd, uint64_t& bytesLeft, uint64_t& writtenBytesTotal, const ProgressCallback& progress);
}