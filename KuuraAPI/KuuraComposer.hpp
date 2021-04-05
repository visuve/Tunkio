#pragma once

#include "KuuraCallbackContainer.hpp"

namespace Kuura
{
	class IWorkload;
	class IFillProvider;

	class Composer
	{
	public:
		Composer(void* context);
		~Composer() = default;

		bool AddTarget(KuuraTargetType type, const std::filesystem::path& path, bool removeAfterOverwrite);
		bool AddPass(KuuraFillType type, bool verify, const char* optional);
		bool Run();

		CallbackContainer Callbacks;
		
		const std::vector<std::shared_ptr<IFillProvider>>& Fillers() const;

	private:
		std::vector<std::shared_ptr<IWorkload>> _workloads;
		std::vector<std::shared_ptr<IFillProvider>> _fillers;
	};
}
