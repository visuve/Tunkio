#pragma once

#include "../TunkioAPI/TunkioAPI.h"

#include <QThread>

class TunkioRunner : public QThread
{
	Q_OBJECT
public:
	explicit TunkioRunner(QString path, TunkioTargetType type, QObject* parent = nullptr);
	~TunkioRunner();

public slots:
	bool addPass(TunkioFillType fillType, const QByteArray& fillValue, bool verify);
	void stop();

signals:
	void wipeStarted(uint16_t passes, uint64_t bytesToWritePerPass);
	void passStarted(uint16_t pass);
	void passProgressed(uint16_t pass, uint64_t bytesWritten);
	void passFinished(uint16_t pass);
	void wipeCompleted(uint16_t passes, uint64_t totalBytesWritten);
	void errorOccurred(TunkioStage stage, uint16_t pass, uint64_t bytesWritten, uint32_t errorCode);

private:
	std::atomic<bool> m_keepRunning;
	void attachCallbacks();
	void run() override;
	QString m_path;
	TunkioTargetType m_type;
	TunkioHandle* m_tunkio = nullptr;
};
