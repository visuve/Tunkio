#pragma once

#include "../KuuraAPI.h"
#include "KuuraWorkload.hpp"

namespace Kuura
{
	class File;

	class FileWorkload : public IWorkload
	{
	public:
		FileWorkload(
			const CallbackContainer* callbacks,
			const std::filesystem::path& path,
			bool removeAfterOverwrite);

		uint64_t Size() override;
		std::pair<bool, uint64_t> Run(const std::vector<std::shared_ptr<IFillProvider>>& fillers) override;

	private:
		std::shared_ptr<File> _file;
	};
}
