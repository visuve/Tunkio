#pragma once

#include "TunkioAPI.h"
#include "TunkioOperation.hpp"

namespace Tunkio
{
	class DeviceWipe : public IOperation
	{
	public:
		DeviceWipe(const TunkioOptions* options);
		bool Run() override;
	};
}