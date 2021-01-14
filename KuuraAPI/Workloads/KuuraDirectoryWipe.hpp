#pragma once

#include "../KuuraAPI.h"
#include "KuuraWorkload.hpp"

namespace Kuura
{
	class DirectoryWipe : public IWorkload
	{
	public:
		DirectoryWipe(const std::filesystem::path& path, bool removeAfterWipe, void* context);
		bool Run() override;
	};
}
