#pragma once

namespace WDW
{
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
        const RawHandle m_handle = INVALID_HANDLE_VALUE;
    };
}