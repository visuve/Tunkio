#pragma once

namespace Tunkio::IO
{
    using RawHandle = void*;

    class AutoHandle
    {
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