#pragma once

#include "../TunkioDLL/TunkioAPI.h"

namespace Tunkio::Memory
{
	struct TunkioDeleter
	{
		void operator()(TunkioHandle* tunkio)
		{
			if (tunkio)
			{
				TunkioFree(tunkio);
			}
		}
	};

	using AutoHandle = std::unique_ptr<TunkioHandle, Tunkio::Memory::TunkioDeleter>;
}