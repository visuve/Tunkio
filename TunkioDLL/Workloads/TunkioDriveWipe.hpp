#pragma once

#include "../TunkioAPI.h"
#include "TunkioOperation.hpp"

namespace Tunkio
{
	class DriveWipe : public IOperation
	{
	public:
		DriveWipe(const std::string& path);
		bool Run() override;
	};
}