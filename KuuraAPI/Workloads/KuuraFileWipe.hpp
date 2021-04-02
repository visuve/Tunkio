#pragma once

#include "../KuuraAPI.h"
#include "KuuraWorkload.hpp"

namespace Kuura
{
	class FileWipe : public IWorkload
	{
	public:
		FileWipe(
			const Composer* parent,
			const std::filesystem::path& path,
			bool removeAfterWipe);

		bool Run() override;
	};
}
