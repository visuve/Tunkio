#pragma once

#include "../KuuraAPI.h"
#include "KuuraWorkload.hpp"

namespace Kuura
{
	class DirectoryWorkload : public IWorkload
	{
	public:
		DirectoryWorkload(
			const Composer* parent,
			const std::filesystem::path& path,
			bool removeAfterOverwrite);

		bool Run() override;
	};
}
