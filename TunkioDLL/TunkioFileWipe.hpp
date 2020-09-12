#pragma once

#include "TunkioAPI.h"
#include "TunkioOperation.hpp"

namespace Tunkio
{
	class FileWipe : public IOperation
	{
	public:
		FileWipe(const TunkioOptions* options);
		bool Run() override;
	};
}