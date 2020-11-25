#pragma once

#include "../TunkioAPI.h"
#include "../FillProviders/TunkioFillProvider.hpp"

namespace Tunkio
{
	class IWorkload
	{
	public:
		IWorkload(const std::filesystem::path& path, bool removeAfterWipe, void* context);
		virtual ~IWorkload();

		virtual bool Run() = 0;

		void SetWipeStartedCallback(TunkioWipeStartedCallback* callback);
		void SetPassStartedCallback(TunkioPassStartedCallback* callback);
		void SetProgressCallback(TunkioProgressCallback* callback);
		void SetPassCompletedCallback(TunkioPassCompletedCallback* callback);
		void SetWipeCompletedCallback(TunkioWipeCompletedCallback* callback);
		void SetErrorCallback(TunkioErrorCallback* callback);

		bool VerifyPass = false;

		template <typename T>
		void AddFiller(T&& provider)
		{
			m_fillers.emplace(provider);
		}

		uint16_t FillerCount() const;
		bool HasFillers() const;

		DataUnit::Bytes ChunkSize() const
		{
			return DataUnit::Mebibytes(1);
		}

	protected:
		std::shared_ptr<IFillProvider> TakeFiller();

		void OnWipeStarted(uint16_t passes, uint64_t bytesToWritePerPass);
		void OnPassStarted(uint16_t pass);
		bool OnProgress(uint16_t pass, uint64_t bytesWritten);
		void OnPassCompleted(uint16_t pass);
		void OnWipeCompleted(uint16_t passes, uint64_t totalBytesWritten);
		void OnError(TunkioStage stage, uint16_t pass, uint64_t bytesWritten, uint32_t errorCode);

		const std::filesystem::path m_path;
		const bool m_removeAfterWipe = false;

	private:
		TunkioWipeStartedCallback* m_wipeStartedCallback = nullptr;
		TunkioPassStartedCallback* m_passStartedCallback = nullptr;
		TunkioProgressCallback* m_progressCallback = nullptr;
		TunkioPassCompletedCallback* m_passCompletedCallback = nullptr;
		TunkioWipeCompletedCallback* m_wipeCompletedCallback = nullptr;
		TunkioErrorCallback* m_errorCallback = nullptr;

		void* m_context = nullptr;
		std::queue<std::shared_ptr<IFillProvider>> m_fillers;
	};
}
