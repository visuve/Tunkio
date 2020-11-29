#pragma once

#include "../TunkioAPI.h"
#include "TunkioWorkload.hpp"

namespace Tunkio
{
	class FileWipe : public IWorkload
	{
	public:
		FileWipe(const std::filesystem::path& path, bool removeAfterWipe, void* context);
		bool Run() override;
	};
}
