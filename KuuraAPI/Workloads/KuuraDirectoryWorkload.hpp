#pragma once

#include "../KuuraAPI.h"
#include "KuuraWorkload.hpp"

namespace Kuura
{
	class Directory;

	class DirectoryWorkload : public IWorkload
	{
	public:
		DirectoryWorkload(
			const CallbackContainer* parent,
			const std::filesystem::path& path,
			bool removeAfterOverwrite);

		uint64_t Size() override;
		std::pair<bool, uint64_t> Run(const std::vector<std::shared_ptr<IFillProvider>>& fillers) override;

	private:
		std::shared_ptr<Directory> _directory;
	};
}
