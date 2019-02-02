#pragma once

#include "Timer.hpp"

#include <Windows.h>
#include <functional>

namespace WDW
{
    constexpr DWORD KiloByte = 1024;
    constexpr DWORD MegaByte = KiloByte * 1024;
    using ProgressCallback = std::function<void(UINT64, std::chrono::seconds)>;

    class AutoHandle
    {
    public:
        AutoHandle(const HANDLE handle);
        AutoHandle(const AutoHandle&) = delete;
        AutoHandle(AutoHandle&&) = delete;
        AutoHandle& operator = (const AutoHandle&) = delete;
        AutoHandle& operator = (AutoHandle&&) = delete;
        ~AutoHandle();

        bool IsValid() const;
        operator const HANDLE() const;
    private:
        HANDLE m_handle = INVALID_HANDLE_VALUE;
    };

    UINT64 DiskSize(const AutoHandle& hdd);
    bool WipeDrive(const AutoHandle& hdd, UINT64& bytesLeft, UINT64& writtenBytesTotal, const ProgressCallback& progress);
}