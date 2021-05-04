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
			KuuraTargetType targetType,
			const std::filesystem::path& targetPath,
			KuuraFillType fillType,
			const std::string& fillValue,
			bool verify,
			bool remove);

		bool Run();
		void Stop();
		int32_t Error() const;

		void OnOverwriteStarted(uint16_t passes, uint64_t bytesLeft);
		void OnTargetStarted(const std::filesystem::path& path, uint64_t bytesLeft);
		void OnPassStarted(const std::filesystem::path& path, uint16_t pass);
		bool OnProgress(const std::filesystem::path&, uint16_t pass, uint64_t bytesWritten);
		void OnPassCompleted(const std::filesystem::path& path, uint16_t pass);
		void OnTargetCompleted(const std::filesystem::path& path, uint64_t bytesWritten);
		void OnError(const std::filesystem::path& path, KuuraStage stage, uint16_t pass, uint64_t bytesWritten, uint32_t error);
		void OnOverwriteCompleted(uint16_t passes, uint64_t bytesWritten);

	private:
		std::atomic<bool> _keepRunning = true;
		KuuraHandle* _handle = nullptr;
		Time::Timer _totalTimer;
		Time::Timer _currentTimer;
		uint32_t _error = ErrorCode::Success;
		uint64_t _bytesToWrite = 0;
		uint64_t _bytesWrittenLastTime = 0;
	};
}
