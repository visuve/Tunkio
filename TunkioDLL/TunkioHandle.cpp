#include "PCH.hpp"
#include "TunkioHandle.hpp"

namespace Tunkio::IO
{
    AutoHandle::AutoHandle(const RawHandle handle) :
        m_handle(handle)
    {
    }

    AutoHandle::~AutoHandle()
    {
        if (m_handle)
        {
            CloseHandle(m_handle);
        }
    }

    bool AutoHandle::IsValid() const
    {
        return m_handle != INVALID_HANDLE_VALUE;
    }

    AutoHandle::operator const RawHandle() const
    {
        return m_handle;
    }
}