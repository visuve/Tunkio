#pragma once

#include "../KuuraAPI/KuuraAPI.h"

#include <QThread>

#include <filesystem>

class KuuraRunner : public QThread
{
	Q_OBJECT
public:
	explicit KuuraRunner(QObject* parent = nullptr);
	~KuuraRunner();

public slots:
	bool addTarget(KuuraTargetType type, const QString& path, bool remove);
	bool addTarget(KuuraTargetType type, const QFileInfo& path, bool remove);
	bool addPass(KuuraFillType fillType, const QByteArray& fillValue, bool verify);
	void stop();

signals:
	void overwriteStarted(uint16_t passes, uint64_t bytesToWritePerPass);
	void targetStarted(const std::filesystem::path& path, uint64_t bytesToWrite);
	void passStarted(const std::filesystem::path& path, uint16_t pass);
	void passProgressed(const std::filesystem::path& path, uint16_t pass, uint64_t bytesWritten);
	void passCompleted(const std::filesystem::path& path, uint16_t pass);
	void targetCompleted(const std::filesystem::path& path, uint64_t bytesWritten);
	void overwriteCompleted(uint16_t passes, uint64_t totalBytesWritten);
	void errorOccurred(const std::filesystem::path& path, KuuraStage stage, uint16_t pass, uint64_t bytesWritten, uint32_t errorCode);

private:
	std::atomic<bool> _keepRunning;
	void attachCallbacks();
	void run() override;
	KuuraHandle* _kuura = nullptr;
};
