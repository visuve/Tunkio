#pragma once

#include <unistd.h>
#include "../TunkioAutoHandle.hpp"

namespace Tunkio
{
	class PosixAutoHandle : public AutoHandle<int, 0, -1, close>
	{
	public:
		// Produces relocation R_X86_64_PC32 linker error in TunkioDLL if moved to .cpp
		PosixAutoHandle() :
			AutoHandle()
		{
		}

		explicit PosixAutoHandle(int handle) :
			AutoHandle(handle)
		{
		}

		~PosixAutoHandle() = default;
	};
}