#pragma once

#include "../KuuraAPI.h"
#include "KuuraWorkload.hpp"

namespace Kuura
{
	class Drive;

	class DriveWorkload : public IWorkload
	{
	public:
		DriveWorkload(const CallbackContainer* callbacks, const std::filesystem::path& path);

		uint64_t Size() override;
		std::pair<bool, uint64_t> Run(const std::vector<std::shared_ptr<IFillProvider>>& fillers) override;

	private:
		std::shared_ptr<Drive> _drive;
	};
}
