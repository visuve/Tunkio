#pragma once

#include "../TunkioDLL/TunkioAPI.h"

namespace Tunkio
{
	class CLI
	{
	public:
		CLI() = default;
		~CLI();

		bool Initialize(
			const std::string& path,
			TunkioTargetType targetType,
			TunkioFillType fillType,
			const std::string& filler,
			bool verify,
			bool remove);

		bool Run();
		void Stop();
		int32_t Error() const;

		void OnWipeStarted(uint16_t passes, uint64_t bytesLeft);
		void OnPassStarted(uint16_t pass);
		bool OnProgress(uint16_t pass, uint64_t bytesWritten);
		void OnPassCompleted(uint16_t pass);
		void OnWipeError(TunkioStage stage, uint16_t pass, uint64_t bytesWritten, uint32_t error);
		void OnWipeCompleted(uint16_t passes, uint64_t bytesWritten);

	private:
		std::atomic<bool> m_keepRunning = true;
		TunkioHandle* m_handle = nullptr;
		Time::Timer m_totalTimer;
		Time::Timer m_currentTimer;
		uint32_t m_error = ErrorCode::Success;
		uint64_t m_bytesToWrite = 0;
		uint64_t m_bytesWrittenLastTime = 0;
	};
}