#pragma once

#include "../TunkioAPI.h"
#include "TunkioFileWipe.hpp"

namespace Tunkio
{
	class DriveWipe : public FileWipe
	{
	public:
		DriveWipe(const std::filesystem::path& path, void* context);
		bool Run() override;
	};
}