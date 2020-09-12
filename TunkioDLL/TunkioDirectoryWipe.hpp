#pragma once

#include "TunkioAPI.h"
#include "TunkioOperation.hpp"

namespace Tunkio
{
	class DirectoryWipe : public IOperation
	{
	public:
		DirectoryWipe(const TunkioOptions* options);
		bool Run() override;
	};
}