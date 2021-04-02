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

		inline void OnOverwriteStarted(uint16_t passes, uint64_t bytesToWritePerPass) const
		{
			if (OverwriteStartedCallback)
			{
				OverwriteStartedCallback(Context, passes, bytesToWritePerPass);
			}
		}

		inline void OnPassStarted(const KuuraChar* path, uint16_t pass) const
		{
			if (PassStartedCallback)
			{
				PassStartedCallback(Context, path, pass);
			}
		}

		inline bool OnProgress(const KuuraChar* path, uint16_t pass, uint64_t bytesWritten) const
		{
			
			return ProgressCallback ? ProgressCallback(Context, path, pass, bytesWritten) : true;
		}

		inline void OnPassCompleted(const KuuraChar* path, uint16_t pass) const
		{
			if (PassCompletedCallback)
			{
				PassCompletedCallback(Context, path, pass);
			}
		}

		inline void OnOverwriteCompleted(uint16_t passes, uint64_t totalBytesWritten) const
		{
			if (OverwriteCompletedCallback)
			{
				OverwriteCompletedCallback(Context, passes, totalBytesWritten);
			}
		}

		inline void OnError(
			const KuuraChar* path,
			KuuraStage stage,
			uint16_t pass,
			uint64_t bytesWritten,
			uint32_t errorCode) const
		{
			if (ErrorCallback)
			{
				ErrorCallback(Context, path, stage, pass, bytesWritten, errorCode);
			}
		}

		void* Context = nullptr;
		KuuraOverwriteStartedCallback* OverwriteStartedCallback = nullptr;
		KuuraPassStartedCallback* PassStartedCallback = nullptr;
		KuuraProgressCallback* ProgressCallback = nullptr;
		KuuraPassCompletedCallback* PassCompletedCallback = nullptr;
		KuuraOverwriteCompletedCallback* OverwriteCompletedCallback = nullptr;
		KuuraErrorCallback* ErrorCallback = nullptr;
	};
}
