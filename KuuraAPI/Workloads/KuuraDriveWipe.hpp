#pragma once

#include "../KuuraAPI.h"
#include "KuuraFileWipe.hpp"

namespace Kuura
{
	class DriveWipe : public FileWipe
	{
	public:
		DriveWipe(const CallbackContainer& callbacks, const std::filesystem::path& path);
		bool Run() override;
	};
}
