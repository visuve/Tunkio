#pragma once

#include "../KuuraAPI.h"
#include "KuuraWorkload.hpp"

namespace Kuura
{
	class FileWorkload : public IWorkload
	{
	public:
		FileWorkload(
			const CallbackContainer* callbacks,
			const std::filesystem::path& path,
			bool removeAfterOverwrite);

		bool Run(const std::vector<std::shared_ptr<IFillProvider>>& fillers) override;
	};
}
