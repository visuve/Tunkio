#include "TunkioRunner.hpp"
#include <QDebug>
#include <atomic>

TunkioRunner::TunkioRunner(QString path, TunkioTargetType type, QObject* parent) :
	QThread(parent),
	m_path(path),
	m_type(type),
	m_tunkio(TunkioInitialize(this, m_path.toUtf8().constData(), m_type))
{
	attachCallbacks();
}

TunkioRunner::~TunkioRunner()
{
	qDebug() << "Destroying...";
	keepRunning = false;
	requestInterruption();
	wait();

	if (m_tunkio)
	{
		TunkioFree(m_tunkio);
	}

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
	if (!m_tunkio)
	{
		qCritical() << "Tunkio not initialized properly!";
		return;
	}

	TunkioSetWipeStartedCallback(m_tunkio, [](
		void* context,
		uint16_t passes,
		uint64_t bytesToWritePerPass)
	{
		qDebug() << "TunkioWipeStartedCallback: " << passes << '/' << bytesToWritePerPass;
		auto self = static_cast<TunkioRunner*>(context);
		Q_ASSERT(self);
		emit self->wipeStarted(passes, bytesToWritePerPass);
	});

	TunkioSetPassStartedCallback(m_tunkio, [](
		void* context,
		uint16_t pass)
	{
		qDebug() << "TunkioPassStartedCallback: " << pass;
		auto self = static_cast<TunkioRunner*>(context);
		Q_ASSERT(self);
		emit self->passStarted(pass);
	});

	TunkioSetProgressCallback(m_tunkio, [](
		void* context,
		uint16_t pass,
		uint64_t bytesWritten)
	{
		qDebug() << "TunkioProgressCallback: " << pass << '/' << bytesWritten;
		auto self = static_cast<TunkioRunner*>(context);
		Q_ASSERT(self);

		emit self->passProgressed(pass, bytesWritten);
		return self->keepRunning.load();
	});

	TunkioSetPassCompletedCallback(m_tunkio, [](
		void* context,
		uint16_t pass)
	{
		qDebug() << "TunkioPassCompletedCallback: " << pass;
		auto self = static_cast<TunkioRunner*>(context);
		Q_ASSERT(self);
		emit self->passFinished(pass);
	});

	TunkioSetWipeCompletedCallback(m_tunkio, [](
		void* context,
		uint16_t passes,
		uint64_t totalBytesWritten)
	{
		qDebug() << "TunkioCompletedCallback: " << passes << '/' << totalBytesWritten;
		auto self = static_cast<TunkioRunner*>(context);
		Q_ASSERT(self);
		emit self->wipeCompleted(passes, totalBytesWritten);
	});

	TunkioSetErrorCallback(m_tunkio, [](
		void* context,
		TunkioStage stage,
		uint16_t pass,
		uint64_t bytesWritten,
		uint32_t errorCode)
	{
		qDebug() << "TunkioErrorCallback: "
			<< stage << '/' << pass << '/' << bytesWritten << '/' << errorCode;

		auto self = static_cast<TunkioRunner*>(context);
		Q_ASSERT(self);
		emit self->errorOccurred(stage, pass, bytesWritten, errorCode);
	});
}

void TunkioRunner::run()
{
	qDebug() << "Started!";
	Q_ASSERT(m_tunkio);
	qDebug() << TunkioRun(m_tunkio);
	qDebug() << "Finished!";
}
