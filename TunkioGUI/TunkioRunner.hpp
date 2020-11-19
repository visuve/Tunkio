#pragma once

#include <QThread>
#include <QObject>

#include <atomic>

#include "TunkioAPI.h"
#include "TunkioInstance.hpp"

class TunkioRunner : public QThread
{
	Q_OBJECT
public:
	TunkioRunner(QString path, TunkioTargetType type, QObject* parent = nullptr);
	~TunkioRunner();

	bool addPass(TunkioFillType fillType, const QString& fillValue, bool verify);
	std::atomic<bool> keepRunning = true;

signals:
	void wipeStarted(uint16_t totalIterations, uint64_t bytesToWritePerIteration);
	void passStarted(uint16_t pass);
	void passProgressed(uint16_t pass, uint64_t bytesWritten);
	void passFinished(uint16_t pass);
	void wipeCompleted(uint16_t totalIterations, uint64_t totalBytesWritten);
	void errorOccurred(TunkioStage stage, uint16_t currentIteration, uint64_t bytesWritten, uint32_t errorCode);

private:
	void attachCallbacks();
	void run() override;
	QString m_path;
	TunkioTargetType m_type;
	Tunkio::Instance m_tunkio;
};

