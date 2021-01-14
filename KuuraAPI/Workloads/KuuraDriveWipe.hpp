#pragma once

#include "../KuuraAPI.h"
#include "KuuraFileWipe.hpp"

namespace Kuura
{
	class DriveWipe : public FileWipe
	{
	public:
		DriveWipe(const std::filesystem::path& path, void* context);
		bool Run() override;
	};
}
