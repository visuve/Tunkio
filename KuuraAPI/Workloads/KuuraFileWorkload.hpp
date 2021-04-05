#pragma once

#include "../KuuraAPI.h"
#include "KuuraWorkload.hpp"

namespace Kuura
{
	class FileWorkload : public IWorkload
	{
	public:
		FileWorkload(
			const Composer* parent,
			const std::filesystem::path& path,
			bool removeAfterOverwrite);

		bool Run() override;
	};
}
