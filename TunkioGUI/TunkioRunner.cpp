#include "TunkioGUI-PCH.hpp"
#include "TunkioRunner.hpp"

TunkioRunner::TunkioRunner(QString path, TunkioTargetType type, bool remove, QObject* parent) :
	QThread(parent),
	m_path(path),
	m_type(type),
	m_tunkio(TunkioInitialize(m_path.toUtf8().constData(), m_type, remove, this))
{
	attachCallbacks();
}

TunkioRunner::~TunkioRunner()
{
	qDebug() << "Destroying...";

	m_keepRunning = false;
	wait();

	if (m_tunkio)
	{
		TunkioFree(m_tunkio);
	}

	qDebug() << "Destroyed.";
}

bool TunkioRunner::addPass(TunkioFillType fillType, const QByteArray& fillValue, bool verify)
{
	Q_ASSERT(m_tunkio);

	const std::string sequence = fillValue.toStdString();

	switch (fillType)
	{
		case TunkioFillType::ZeroFill:
		case TunkioFillType::OneFill:
		case TunkioFillType::RandomFill:
			return TunkioAddWipeRound(m_tunkio, fillType, verify, nullptr);
		case TunkioFillType::ByteFill:
		{
			char character[] = { sequence[0], '\0' };
			return TunkioAddWipeRound(m_tunkio, fillType, verify, character);
		}
		case TunkioFillType::SequenceFill:
		case TunkioFillType::FileFill:
			return TunkioAddWipeRound(m_tunkio, fillType, verify, sequence.c_str());
	}

	return false;
}

void TunkioRunner::stop()
{
	m_keepRunning = false;
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
		auto self = static_cast<TunkioRunner*>(context);
		Q_ASSERT(self);
		emit self->wipeStarted(passes, bytesToWritePerPass);
	});

	TunkioSetPassStartedCallback(m_tunkio, [](
		void* context,
		uint16_t pass)
	{
		auto self = static_cast<TunkioRunner*>(context);
		Q_ASSERT(self);
		emit self->passStarted(pass);
	});

	TunkioSetProgressCallback(m_tunkio, [](
		void* context,
		uint16_t pass,
		uint64_t bytesWritten)
	{
		auto self = static_cast<TunkioRunner*>(context);
		Q_ASSERT(self);
		emit self->passProgressed(pass, bytesWritten);
		return self->m_keepRunning.load(); // TODO: investigate why isInterruptionRequested() does not work
	});

	TunkioSetPassCompletedCallback(m_tunkio, [](
		void* context,
		uint16_t pass)
	{
		auto self = static_cast<TunkioRunner*>(context);
		Q_ASSERT(self);
		emit self->passFinished(pass);
	});

	TunkioSetWipeCompletedCallback(m_tunkio, [](
		void* context,
		uint16_t passes,
		uint64_t totalBytesWritten)
	{
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
		auto self = static_cast<TunkioRunner*>(context);
		Q_ASSERT(self);
		emit self->errorOccurred(stage, pass, bytesWritten, errorCode);
	});
}

void TunkioRunner::run()
{
	qDebug() << "Started!";
	Q_ASSERT(m_tunkio);

	m_keepRunning = true;

	if (!TunkioRun(m_tunkio))
	{
		qDebug() << "Failed!";
		return;
	}

	qDebug() << (m_keepRunning ? "Finished!" : "Canceled.");
}
