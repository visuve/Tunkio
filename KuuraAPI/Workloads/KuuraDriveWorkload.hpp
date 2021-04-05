#pragma once

#include "../KuuraAPI.h"
#include "KuuraWorkload.hpp"

namespace Kuura
{
	class DriveWorkload : public IWorkload
	{
	public:
		DriveWorkload(const Composer* parent, const std::filesystem::path& path);
		bool Run() override;
	};
}
