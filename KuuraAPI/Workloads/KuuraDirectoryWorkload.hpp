#pragma once

#include "../KuuraAPI.h"
#include "KuuraWorkload.hpp"

namespace Kuura
{
	class DirectoryWorkload : public IWorkload
	{
	public:
		DirectoryWorkload(
			const CallbackContainer* parent,
			const std::filesystem::path& path,
			bool removeAfterOverwrite);

		bool Run(const std::vector<std::shared_ptr<IFillProvider>>& fillers) override;
	};
}
