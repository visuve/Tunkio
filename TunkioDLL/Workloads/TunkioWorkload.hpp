#pragma once

#include "../FillProviders/TunkioFillProvider.hpp"

namespace Tunkio
{
	class IWorkload
	{
	public:
		IWorkload(void* context, const std::string& path) :
			m_context(context),
			m_path(path)
		{
		}

		virtual ~IWorkload()
		{
			// assert(m_fillers.empty()); // Untrue if errors have occurred
		}

		virtual bool Run() = 0;

		std::queue<std::shared_ptr<IFillProvider>> m_fillers;

		// TODO: add setters :S
		TunkioStartedCallback* m_startedCallback = nullptr;
		TunkioIterationStartedCallback* m_iterationStartedCallback = nullptr;
		TunkioProgressCallback* m_progressCallback = nullptr;
		TunkioErrorCallback* m_errorCallback = nullptr;
		TunkioIterationCompletedCallback* m_iterationCompletedCallback = nullptr;
		TunkioCompletedCallback* m_completedCallback = nullptr;

		bool m_verifyAfterWipe = false;
		bool m_removeAfterFill = false;

		inline void OnStarted(
			uint16_t totalIterations,
			uint64_t bytesToWritePerIteration)
		{
			if (!m_startedCallback)
			{
				return;
			}

			m_startedCallback(m_context, totalIterations, bytesToWritePerIteration);
		}

		inline void OnIterationStarted(uint16_t currentIteration)
		{
			if (!m_iterationStartedCallback)
			{
				return;
			}

			m_iterationStartedCallback(m_context, currentIteration);
		}

		inline bool OnProgress(
			uint16_t currentIteration,
			uint64_t bytesWritten)
		{
			if (!m_progressCallback)
			{
				return true;
			}

			return m_progressCallback(m_context, currentIteration, bytesWritten);
		}

		inline void OnIterationCompleted(uint16_t currentIteration)
		{
			if (!m_iterationCompletedCallback)
			{
				return;
			}

			m_iterationCompletedCallback(m_context, currentIteration);
		}

		inline void OnCompleted(
			uint16_t totalIterations,
			uint64_t totalBytesWritten)
		{
			if (!m_completedCallback)
			{
				return;
			}

			m_completedCallback(m_context, totalIterations, totalBytesWritten);
		}

		inline void OnError(
			TunkioStage stage,
			uint16_t currentIteration,
			uint64_t bytesWritten,
			uint32_t errorCode)
		{
			if (!m_errorCallback)
			{
				return;
			}

			m_errorCallback(m_context, stage, currentIteration, bytesWritten, errorCode);
		}

	private:
		void* m_context = nullptr;
	protected:
		const std::string m_path;
	};
}
