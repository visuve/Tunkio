#pragma once
#include "KuuraAPI.h"

namespace Kuura
{
	struct CallbackContainer
	{
		CallbackContainer(void* context) :
			Context(context)
		{
		}

		inline void OnWipeStarted(uint16_t passes, uint64_t bytesToWritePerPass) const
		{
			WipeStartedCallback(Context, passes, bytesToWritePerPass);
		}

		inline void OnPassStarted(const char* path, uint16_t pass) const
		{
			PassStartedCallback(Context, path, pass);
		}

		inline bool OnProgress(const char* path, uint16_t pass, uint64_t bytesWritten) const
		{
			return ProgressCallback(Context, path, pass, bytesWritten);
		}

		inline void OnPassCompleted(const char* path, uint16_t pass) const
		{
			PassCompletedCallback(Context, path, pass);
		}

		inline void OnWipeCompleted(uint16_t passes, uint64_t totalBytesWritten) const
		{
			WipeCompletedCallback(Context, passes, totalBytesWritten);
		}

		inline void OnError(
			const char* path,
			KuuraStage stage,
			uint16_t pass,
			uint64_t bytesWritten,
			uint32_t errorCode) const
		{
			ErrorCallback(Context, path, stage, pass, bytesWritten, errorCode);
		}

		void* Context = nullptr;
		KuuraWipeStartedCallback* WipeStartedCallback = nullptr;
		KuuraPassStartedCallback* PassStartedCallback = nullptr;
		KuuraProgressCallback* ProgressCallback = nullptr;
		KuuraPassCompletedCallback* PassCompletedCallback = nullptr;
		KuuraWipeCompletedCallback* WipeCompletedCallback = nullptr;
		KuuraErrorCallback* ErrorCallback = nullptr;
	};
}
