#pragma once

#include "TunkioAPI.h"
#include "TunkioOperation.hpp"

namespace Tunkio
{
	class DirectoryWipe : public IOperation
	{
	public:
		DirectoryWipe(const std::string& path);
		bool Run() override;
	};
}