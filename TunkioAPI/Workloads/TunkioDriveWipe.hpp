#pragma once

#include "../TunkioAPI.h"
#include "TunkioFileWipe.hpp"

namespace Tunkio
{
	class DriveWipe : public FileWipe
	{
	public:
		DriveWipe(void* context, const std::filesystem::path& path);
		bool Run() override;
	};
}