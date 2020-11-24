#pragma once

#include "../TunkioAPI.h"
#include "TunkioWorkload.hpp"

namespace Tunkio
{
	class DirectoryWipe : public IWorkload
	{
	public:
		DirectoryWipe(void* context, const std::filesystem::path& path);
		bool Run() override;
	};
}