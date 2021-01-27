#pragma once

#include "../KuuraAPI.h"
#include "../FillProviders/KuuraFillProvider.hpp"
#include "../FillConsumers/KuuraFillConsumer.hpp"

namespace Kuura
{
	class IWorkload
	{
	public:
		IWorkload(const std::filesystem::path& path, bool removeAfterWipe, void* context);
		virtual ~IWorkload();

		virtual bool Run() = 0;

		void SetWipeStartedCallback(KuuraWipeStartedCallback* callback);
		void SetPassStartedCallback(KuuraPassStartedCallback* callback);
		void SetProgressCallback(KuuraProgressCallback* callback);
		void SetPassCompletedCallback(KuuraPassCompletedCallback* callback);
		void SetWipeCompletedCallback(KuuraWipeCompletedCallback* callback);
		void SetErrorCallback(KuuraErrorCallback* callback);

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

		void OnWipeStarted(uint16_t passes, uint64_t bytesToWritePerPass);
		void OnPassStarted(uint16_t pass);
		bool OnProgress(uint16_t pass, uint64_t bytesWritten);
		void OnPassCompleted(uint16_t pass);
		void OnWipeCompleted(uint16_t passes, uint64_t totalBytesWritten);
		void OnError(KuuraStage stage, uint16_t pass, uint64_t bytesWritten, uint32_t errorCode);

		const std::filesystem::path m_path;
		const bool m_removeAfterWipe = false;


		bool Fill(
			uint16_t pass,
			uint64_t bytesLeft,
			uint64_t& bytesWritten,
			std::shared_ptr<IFillProvider> filler,
			std::shared_ptr<IFillConsumer> fillable);

	private:
		KuuraWipeStartedCallback* m_wipeStartedCallback = nullptr;
		KuuraPassStartedCallback* m_passStartedCallback = nullptr;
		KuuraProgressCallback* m_progressCallback = nullptr;
		KuuraPassCompletedCallback* m_passCompletedCallback = nullptr;
		KuuraWipeCompletedCallback* m_wipeCompletedCallback = nullptr;
		KuuraErrorCallback* m_errorCallback = nullptr;

		void* m_context = nullptr;
		std::queue<std::shared_ptr<IFillProvider>> m_fillers;
	};
}
