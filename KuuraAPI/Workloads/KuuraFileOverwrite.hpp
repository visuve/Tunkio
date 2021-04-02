#pragma once

#include "../KuuraAPI.h"
#include "KuuraWorkload.hpp"

namespace Kuura
{
	class FileOverwrite : public IWorkload
	{
	public:
		FileOverwrite(
			const Composer* parent,
			const std::filesystem::path& path,
			bool removeAfterOverwrite);

		bool Run() override;
	};
}
