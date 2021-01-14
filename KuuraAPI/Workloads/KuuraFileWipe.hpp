#pragma once

#include "../KuuraAPI.h"
#include "KuuraWorkload.hpp"

namespace Kuura
{
	class FileWipe : public IWorkload
	{
	public:
		FileWipe(const std::filesystem::path& path, bool removeAfterWipe, void* context);
		bool Run() override;
	};
}
