#pragma once

#include "../KuuraAPI.h"
#include "../KuuraComposer.hpp"
#include "../FillProviders/KuuraFillProvider.hpp"
#include "../FillConsumers/KuuraFillConsumer.hpp"

namespace Kuura
{
	class IWorkload
	{
	public:
		IWorkload(
			const Composer* parent,
			const std::filesystem::path& path,
			bool removeAfterWipe);

		virtual ~IWorkload();

		virtual bool Run() = 0;

		bool VerifyPass = false;

	protected:
		const std::filesystem::path m_path;
		const bool m_removeAfterWipe = false;

		bool Fill(
			uint16_t pass,
			uint64_t bytesLeft,
			uint64_t& bytesWritten,
			std::shared_ptr<IFillProvider> filler,
			std::shared_ptr<IFillConsumer> fillable);

		const Composer* m_parent;

	private:
		std::queue<std::shared_ptr<IFillProvider>> m_fillers;
	};
}
