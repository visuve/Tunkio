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

		bool AddWorkload(const char* path, KuuraTargetType type, bool removeAfterWipe);
		bool AddFiller(KuuraFillType type, bool verify, const char* optional);
		bool Run();

		CallbackContainer Callbacks;
		
		const std::vector<std::shared_ptr<IWorkload>>& Workloads() const;
		const std::vector<std::shared_ptr<IFillProvider>>& Fillers() const;

	private:
		std::vector<std::shared_ptr<IWorkload>> m_workloads;
		std::vector<std::shared_ptr<IFillProvider>> m_fillers;
	};
}
