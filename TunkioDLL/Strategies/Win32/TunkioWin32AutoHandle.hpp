#pragma once

#include <Windows.h>

namespace Tunkio
{
    class Win32AutoHandle
    {
    public:
        Win32AutoHandle(const HANDLE handle);
        ~Win32AutoHandle();

        operator const HANDLE() const;
        bool IsValid() const;
    private:
        const HANDLE m_handle = INVALID_HANDLE_VALUE;
    };
}