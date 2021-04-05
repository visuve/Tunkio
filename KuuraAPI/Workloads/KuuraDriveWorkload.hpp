#pragma once

#include "../KuuraAPI.h"
#include "KuuraWorkload.hpp"

namespace Kuura
{
	class DriveWorkload : public IWorkload
	{
	public:
		DriveWorkload(const CallbackContainer* callbacks, const std::filesystem::path& path);
		bool Run(const std::vector<std::shared_ptr<IFillProvider>>& fillers) override;
	};
}
