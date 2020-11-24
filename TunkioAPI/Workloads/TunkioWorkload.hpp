#pragma once

#include "../FillProviders/TunkioFillProvider.hpp"

namespace Tunkio
{
	class IWorkload
	{
	public:
		IWorkload(void* context, const std::filesystem::path& path) :
			m_context(context),
			m_path(path)
		{
		}

		virtual ~IWorkload()
		{
		}

		virtual bool Run() = 0;

		std::queue<std::shared_ptr<IFillProvider>> m_fillers;

		// TODO: add setters :S
		TunkioWipeStartedCallback* m_startedCallback = nullptr;
		TunkioPassStartedCallback* m_passStartedCallback = nullptr;
		TunkioProgressCallback* m_progressCallback = nullptr;
		TunkioErrorCallback* m_errorCallback = nullptr;
		TunkioPassCompletedCallback* m_passCompletedCallback = nullptr;
		TunkioCompletedCallback* m_completedCallback = nullptr;

		bool m_verifyAfterWipe = false;
		bool m_removeAfterFill = false;

		inline void OnWipeStarted(
			uint16_t passes,
			uint64_t bytesToWritePerPass)
		{
			if (!m_startedCallback)
			{
				return;
			}

			m_startedCallback(m_context, passes, bytesToWritePerPass);
		}

		inline void OnPassStarted(uint16_t pass)
		{
			if (!m_passStartedCallback)
			{
				return;
			}

			m_passStartedCallback(m_context, pass);
		}

		inline bool OnProgress(
			uint16_t pass,
			uint64_t bytesWritten)
		{
			if (!m_progressCallback)
			{
				return true;
			}

			return m_progressCallback(m_context, pass, bytesWritten);
		}

		inline void OnPassCompleted(uint16_t pass)
		{
			if (!m_passCompletedCallback)
			{
				return;
			}

			m_passCompletedCallback(m_context, pass);
		}

		inline void OnCompleted(
			uint16_t passes,
			uint64_t totalBytesWritten)
		{
			if (!m_completedCallback)
			{
				return;
			}

			m_completedCallback(m_context, passes, totalBytesWritten);
		}

		inline void OnWipeError(
			TunkioStage stage,
			uint16_t pass,
			uint64_t bytesWritten,
			uint32_t errorCode)
		{
			if (!m_errorCallback)
			{
				return;
			}

			m_errorCallback(m_context, stage, pass, bytesWritten, errorCode);
		}

	private:
		void* m_context = nullptr;
	protected:
		const std::filesystem::path m_path;
	};
}
