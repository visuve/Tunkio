#include "PCH.hpp"
#include "TunkioWin32AutoHandle.hpp"

namespace Tunkio
{
    Win32AutoHandle::Win32AutoHandle(const HANDLE handle) :
        m_handle(handle)
    {
    }

    Win32AutoHandle::~Win32AutoHandle()
    {
        if (m_handle)
        {
            CloseHandle(m_handle);
        }
    }

    Win32AutoHandle::operator const HANDLE() const
    {
        return m_handle;
    }

    bool Win32AutoHandle::IsValid() const
    {
        return m_handle && m_handle != INVALID_HANDLE_VALUE;
    }
}