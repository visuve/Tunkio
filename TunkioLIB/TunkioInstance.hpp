#pragma once

#include "../TunkioDLL/TunkioAPI.h"

namespace Tunkio
{
	class Instance
	{
	public:
		Instance() = default;
		explicit Instance(void* context, const char* path, TunkioTargetType type);
		~Instance();

		TunkioHandle* Get() const;
		operator TunkioHandle* ();

	private:
		TunkioHandle* m_handle = nullptr;
	};
}
