#pragma once

#include "../TunkioAPI.h"
#include "TunkioWorkload.hpp"

namespace Tunkio
{
	class FileWipe : public IWorkload
	{
	public:
		FileWipe(const std::string& path);
		bool Run() override;
	};
}