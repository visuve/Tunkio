#pragma once

#include "../KuuraAPI/KuuraAPI.h"
#include "../KuuraLIB/KuuraErrorCodes.hpp"

namespace Kuura
{
	class CLI
	{
	public:
		CLI() = default;
		~CLI();

		bool Initialize(
			const std::filesystem::path& path,
			KuuraTargetType targetType,
			KuuraFillType fillType,
			const std::string& filler,
			bool verify,
			bool remove);

		bool Run();
		void Stop();
		int32_t Error() const;

		void OnOverwriteStarted(uint16_t passes, uint64_t bytesLeft);
		void OnPassStarted(const std::filesystem::path& path, uint16_t pass);
		bool OnProgress(const std::filesystem::path&, uint16_t pass, uint64_t bytesWritten);
		void OnPassCompleted(const std::filesystem::path& path, uint16_t pass);
		void OnError(const std::filesystem::path& path, KuuraStage stage, uint16_t pass, uint64_t bytesWritten, uint32_t error);
		void OnOverwriteCompleted(uint16_t passes, uint64_t bytesWritten);

	private:
		std::atomic<bool> m_keepRunning = true;
		KuuraHandle* m_handle = nullptr;
		Time::Timer m_totalTimer;
		Time::Timer m_currentTimer;
		uint32_t m_error = ErrorCode::Success;
		uint64_t m_bytesToWrite = 0;
		uint64_t m_bytesWrittenLastTime = 0;
	};
}
