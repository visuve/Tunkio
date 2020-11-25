#pragma once

#include "../TunkioAPI.h"
#include "TunkioWorkload.hpp"

namespace Tunkio
{
	class DirectoryWipe : public IWorkload
	{
	public:
		DirectoryWipe(const std::filesystem::path& path, bool removeAfterWipe, void* context);
		bool Run() override;
	};
}