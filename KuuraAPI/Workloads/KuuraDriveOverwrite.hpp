#pragma once

#include "../KuuraAPI.h"
#include "KuuraFileOverwrite.hpp"

namespace Kuura
{
	class DriveOverwrite : public FileOverwrite
	{
	public:
		DriveOverwrite(const Composer* parent, const std::filesystem::path& path);
		bool Run() override;
	};
}
