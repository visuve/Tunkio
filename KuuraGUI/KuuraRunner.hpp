#pragma once

#include "../KuuraAPI/KuuraAPI.h"

#include <QThread>

class KuuraRunner : public QThread
{
	Q_OBJECT
public:
	explicit KuuraRunner(QObject* parent = nullptr);
	~KuuraRunner();

public slots:
	bool addTarget(const std::filesystem::path& path, KuuraTargetType type, bool remove);
	bool addPass(KuuraFillType fillType, const QByteArray& fillValue, bool verify);
	void stop();

signals:
	void wipeStarted(uint16_t passes, uint64_t bytesToWritePerPass);
	void passStarted(const std::filesystem::path& path, uint16_t pass);
	void passProgressed(const std::filesystem::path& path, uint16_t pass, uint64_t bytesWritten);
	void passFinished(const std::filesystem::path& path, uint16_t pass);
	void wipeCompleted(uint16_t passes, uint64_t totalBytesWritten);
	void errorOccurred(const std::filesystem::path& path, KuuraStage stage, uint16_t pass, uint64_t bytesWritten, uint32_t errorCode);

private:
	std::atomic<bool> _keepRunning;
	void attachCallbacks();
	void run() override;
	KuuraHandle* _kuura = nullptr;
};
