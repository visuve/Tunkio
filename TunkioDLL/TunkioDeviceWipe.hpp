#pragma once

#include "TunkioAPI.h"
#include "TunkioOperation.hpp"

namespace Tunkio
{
	class DeviceWipe : public IOperation
	{
	public:
		DeviceWipe(const std::string& path);
		bool Run() override;
	};
}