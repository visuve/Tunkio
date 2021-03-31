#pragma once

#include "../KuuraAPI/KuuraAPI.h"

#include <QThread>

class KuuraRunner : public QThread
{
	Q_OBJECT
public:
	explicit KuuraRunner(QString path, KuuraTargetType type, bool remove, QObject* parent = nullptr);
	~KuuraRunner();

public slots:
	bool addPass(KuuraFillType fillType, const QByteArray& fillValue, bool verify);
	void stop();

signals:
	void wipeStarted(uint16_t passes, uint64_t bytesToWritePerPass);
	void passStarted(const QString& path, uint16_t pass);
	void passProgressed(const QString& path, uint16_t pass, uint64_t bytesWritten);
	void passFinished(const QString& path, uint16_t pass);
	void wipeCompleted(uint16_t passes, uint64_t totalBytesWritten);
	void errorOccurred(const QString& path, KuuraStage stage, uint16_t pass, uint64_t bytesWritten, uint32_t errorCode);

private:
	std::atomic<bool> m_keepRunning;
	void attachCallbacks();
	void run() override;
	QString m_path;
	KuuraTargetType m_type;
	KuuraHandle* m_kuura = nullptr;
};
