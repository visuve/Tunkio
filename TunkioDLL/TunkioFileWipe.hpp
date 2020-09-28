#pragma once

#include "TunkioAPI.h"
#include "TunkioOperation.hpp"

namespace Tunkio
{
	class FileWipe : public IOperation
	{
	public:
		FileWipe(const std::string& path);
		bool Run() override;
	};
}