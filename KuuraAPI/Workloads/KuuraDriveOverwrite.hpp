#pragma once

#include "../KuuraAPI.h"
#include "KuuraWorkload.hpp"

namespace Kuura
{
	class DriveOverwrite : public IWorkload
	{
	public:
		DriveOverwrite(const Composer* parent, const std::filesystem::path& path);
		bool Run() override;
	};
}
