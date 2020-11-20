#pragma once

#include "../TunkioAPI.h"
#include "TunkioWorkload.hpp"

namespace Tunkio
{
	class FileWipe : public IWorkload
	{
	public:
		FileWipe(void* context, const std::string& path);
		bool Run() override;
	};
}