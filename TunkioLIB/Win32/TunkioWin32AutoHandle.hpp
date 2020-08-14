#pragma once

#include "../TunkioAutoHandle.hpp"

namespace Tunkio
{
	class Win32AutoHandle : public AutoHandle<HANDLE, nullptr, INVALID_HANDLE_VALUE, CloseHandle>
	{
	public:
		Win32AutoHandle();
		explicit Win32AutoHandle(HANDLE handle);
		~Win32AutoHandle() = default;

		PHANDLE PHandle();
	};
}