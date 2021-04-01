#pragma once

#include "../KuuraAPI.h"
#include "../KuuraCallbackContainer.hpp"
#include "../FillProviders/KuuraFillProvider.hpp"
#include "../FillConsumers/KuuraFillConsumer.hpp"

namespace Kuura
{
	

	class IWorkload
	{
	public:
		IWorkload(
			const CallbackContainer& callbacks,
			const std::filesystem::path& path,
			bool removeAfterWipe);

		virtual ~IWorkload();

		virtual bool Run() = 0;

		bool VerifyPass = false;

		template <typename T>
		void AddFiller(T&& provider)
		{
			m_fillers.emplace(provider);
		}

		uint16_t FillerCount() const;
		bool HasFillers() const;

	protected:
		std::shared_ptr<IFillProvider> TakeFiller();



		const std::filesystem::path m_path;
		const bool m_removeAfterWipe = false;

		bool Fill(
			uint16_t pass,
			uint64_t bytesLeft,
			uint64_t& bytesWritten,
			std::shared_ptr<IFillProvider> filler,
			std::shared_ptr<IFillConsumer> fillable);

		const CallbackContainer& m_callbacks;

	private:
		std::queue<std::shared_ptr<IFillProvider>> m_fillers;
	};
}
