#include "../PCH.hpp"
#include "TunkioWin32AutoHandle.hpp"

namespace Tunkio
{
	Win32AutoHandle::Win32AutoHandle(const HANDLE handle) :
		m_handle(handle)
	{
	}

	Win32AutoHandle::~Win32AutoHandle()
	{
		Reset();
	}

	void Win32AutoHandle::Reset(HANDLE handle)
	{
		if (m_handle == handle)
		{
			return;
		}

		if (m_handle)
		{
			CloseHandle(m_handle);
		}

		m_handle = handle;
	}

	const HANDLE Win32AutoHandle::Handle() const
	{
		return m_handle;
	}

	PHANDLE Win32AutoHandle::PHandle()
	{
		return &m_handle;
	}

	bool Win32AutoHandle::IsValid() const
	{
		return m_handle && m_handle != INVALID_HANDLE_VALUE;
	}
}