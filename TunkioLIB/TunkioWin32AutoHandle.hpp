#pragma once

namespace Tunkio
{
    class Win32AutoHandle
    {
    public:
        Win32AutoHandle() = default;
        Win32AutoHandle(const HANDLE handle);
        ~Win32AutoHandle();

        void Reset(HANDLE handle = nullptr);
        const HANDLE Handle() const;
        PHANDLE PHandle();
        bool IsValid() const;
    private:
        HANDLE m_handle = INVALID_HANDLE_VALUE;
    };
}