#pragma once

#include "../KuuraAPI.h"
#include "KuuraWorkload.hpp"

namespace Kuura
{
	class DirectoryWipe : public IWorkload
	{
	public:
		DirectoryWipe(
			const CallbackContainer& callbacks,
			const std::filesystem::path& path,
			bool removeAfterWipe);

		bool Run() override;
	};
}
