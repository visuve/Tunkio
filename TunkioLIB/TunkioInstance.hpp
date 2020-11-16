#pragma once

#include "../TunkioDLL/TunkioAPI.h"
#include "TunkioAutoHandle.hpp"

namespace Tunkio
{
	class Instance : public AutoHandle<TunkioHandle*, nullptr, nullptr, TunkioFree>
	{
	public:
		Instance();
		explicit Instance(void* context, const char* path, TunkioTargetType type);

		TunkioHandle* Get() const;
		operator TunkioHandle* ();
	};
}
