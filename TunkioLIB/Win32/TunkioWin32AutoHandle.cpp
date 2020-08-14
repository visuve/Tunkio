#include "../PCH.hpp"
#include "TunkioWin32AutoHandle.hpp"

namespace Tunkio
{
	Win32AutoHandle::Win32AutoHandle() :
		AutoHandle()
	{
	}

	Win32AutoHandle::Win32AutoHandle(HANDLE handle) :
		AutoHandle(handle)
	{
	}

	PHANDLE Win32AutoHandle::PHandle()
	{
		return &m_handle;
	}
}