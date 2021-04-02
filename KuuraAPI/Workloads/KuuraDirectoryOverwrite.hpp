#pragma once

#include "../KuuraAPI.h"
#include "KuuraWorkload.hpp"

namespace Kuura
{
	class DirectoryOverwrite : public IWorkload
	{
	public:
		DirectoryOverwrite(
			const Composer* parent,
			const std::filesystem::path& path,
			bool removeAfterOverwrite);

		bool Run() override;
	};
}
