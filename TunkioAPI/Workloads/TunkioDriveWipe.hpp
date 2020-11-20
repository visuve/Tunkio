#pragma once

#include "../TunkioAPI.h"
#include "TunkioFileWipe.hpp"

namespace Tunkio
{
	class DriveWipe : public FileWipe
	{
	public:
		DriveWipe(void* context, const std::string& path);
		bool Run() override;
	};
}