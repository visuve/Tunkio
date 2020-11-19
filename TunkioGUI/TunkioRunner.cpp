#include "TunkioRunner.hpp"
#include <QDebug>
#include <atomic>

TunkioRunner::TunkioRunner(QString path, TunkioTargetType type, QObject* parent) :
	QThread(parent),
	m_path(path),
	m_type(type),
	m_tunkio(this, m_path.toUtf8().constData(), m_type)
{
	attachCallbacks();
}

TunkioRunner::~TunkioRunner()
{
	qDebug() << "Destroying...";
	keepRunning = false;
	requestInterruption();
	wait();
	qDebug() << "Destroyed.";
}

bool TunkioRunner::addPass(TunkioFillType fillType, const QString &fillValue, bool verify)
{
	const std::string sentence = fillValue.toStdString();
	const char* character = &sentence[0];

	switch (fillType)
	{
		case TunkioFillType::Character:
			return TunkioAddWipeRound(m_tunkio, fillType, verify, sentence.c_str());
		case TunkioFillType::Sentence:
			return TunkioAddWipeRound(m_tunkio, fillType, verify, character);
		default:
			return TunkioAddWipeRound(m_tunkio, fillType, verify, nullptr);
	}
}

void TunkioRunner::attachCallbacks()
{
	TunkioStartedCallback* started = [](
		void* context,
		uint16_t totalIterations,
		uint64_t bytesToWritePerIteration)
	{
		qDebug() << "TunkioStartedCallback: " << totalIterations << '/' << bytesToWritePerIteration;
		auto self = static_cast<TunkioRunner*>(context);
		Q_ASSERT(self);
		emit self->wipeStarted(totalIterations, bytesToWritePerIteration);
	};

	TunkioIterationStartedCallback* iterationStarted = [](
		void* context,
		uint16_t currentIteration)
	{
		qDebug() << "TunkioIterationStartedCallback: " << currentIteration;
		auto self = static_cast<TunkioRunner*>(context);
		Q_ASSERT(self);
		emit self->passStarted(currentIteration);
	};

	TunkioProgressCallback* progress = [](
		void* context,
		uint16_t currentIteration,
		uint64_t bytesWritten)
	{
		qDebug() << "TunkioProgressCallback: " << currentIteration << '/' << bytesWritten;
		auto self = static_cast<TunkioRunner*>(context);
		Q_ASSERT(self);

		emit self->passProgressed(currentIteration, bytesWritten);
		// return self->keepRunning.load();
		return true;
	};

	TunkioIterationCompletedCallback* iterationCompleted = [](
		void* context,
		uint16_t currentIteration)
	{
		qDebug() << "TunkioIterationCompletedCallback: " << currentIteration;
		auto self = static_cast<TunkioRunner*>(context);
		Q_ASSERT(self);
		emit self->passFinished(currentIteration);
	};

	TunkioCompletedCallback* completed = [](
		void* context,
		uint16_t totalIterations,
		uint64_t totalBytesWritten)
	{
		qDebug() << "TunkioCompletedCallback: " << totalIterations << '/' << totalBytesWritten;
		auto self = static_cast<TunkioRunner*>(context);
		Q_ASSERT(self);
		emit self->wipeCompleted(totalIterations, totalBytesWritten);
	};

	TunkioErrorCallback* error = [](
		void* context,
		TunkioStage stage,
		uint16_t currentIteration,
		uint64_t bytesWritten,
		uint32_t errorCode)
	{
		qDebug() << "TunkioErrorCallback: "
			<< stage << '/' << currentIteration << '/' << bytesWritten << '/' << errorCode;

		auto self = static_cast<TunkioRunner*>(context);
		Q_ASSERT(self);
		emit self->errorOccurred(stage, currentIteration, bytesWritten, errorCode);
	};

	qDebug() << TunkioSetStartedCallback(m_tunkio, started);
	qDebug() << TunkioSetIterationStartedCallback(m_tunkio, iterationStarted);
	qDebug() << TunkioSetProgressCallback(m_tunkio, progress);
	qDebug() << TunkioSetIterationCompletedCallback(m_tunkio, iterationCompleted);
	qDebug() << TunkioSetCompletedCallback(m_tunkio, completed);
	qDebug() << TunkioSetErrorCallback(m_tunkio, error);
}

void TunkioRunner::run()
{
	qDebug() << "Started!";
	Q_ASSERT(m_tunkio);
	qDebug() << TunkioRun(m_tunkio);
	qDebug() << "Finished!";
}
