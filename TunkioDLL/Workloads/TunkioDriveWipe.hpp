#pragma once

#include "../TunkioAPI.h"
#include "TunkioWorkload.hpp"

namespace Tunkio
{
	class DriveWipe : public IWorkload
	{
	public:
		DriveWipe(const std::string& path);
		bool Run() override;
	};
}