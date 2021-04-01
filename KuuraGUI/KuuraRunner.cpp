#include "KuuraGUI-PCH.hpp"
#include "KuuraRunner.hpp"

KuuraRunner::KuuraRunner(QObject* parent) :
	QThread(parent),
	m_kuura(KuuraInitialize(this))
{
}

KuuraRunner::~KuuraRunner()
{
	qDebug() << "Destroying...";

	m_keepRunning = false;
	wait();

	if (m_kuura)
	{
		KuuraFree(m_kuura);
	}

	qDebug() << "Destroyed.";
}

bool KuuraRunner::addTarget(const QString& path, KuuraTargetType type, bool remove)
{
	const auto tmp = path.toStdString();
	return KuuraAddTarget(m_kuura, tmp.c_str(), type, remove);
}

bool KuuraRunner::addPass(KuuraFillType fillType, const QByteArray& fillValue, bool verify)
{
	Q_ASSERT(m_kuura);

	const std::string sequence = fillValue.toStdString();

	switch (fillType)
	{
		case KuuraFillType::ZeroFill:
		case KuuraFillType::OneFill:
		case KuuraFillType::RandomFill:
			return KuuraAddWipeRound(m_kuura, fillType, verify, nullptr);
		case KuuraFillType::ByteFill:
		{
			char character[] = { sequence[0], '\0' };
			return KuuraAddWipeRound(m_kuura, fillType, verify, character);
		}
		case KuuraFillType::SequenceFill:
		case KuuraFillType::FileFill:
			return KuuraAddWipeRound(m_kuura, fillType, verify, sequence.c_str());
	}

	return false;
}

void KuuraRunner::stop()
{
	m_keepRunning = false;
}

void KuuraRunner::attachCallbacks()
{
	if (!m_kuura)
	{
		qCritical() << "Kuura not initialized properly!";
		return;
	}

	KuuraSetWipeStartedCallback(m_kuura, [](
		void* context,
		uint16_t passes,
		uint64_t bytesToWritePerPass)
	{
		auto self = static_cast<KuuraRunner*>(context);
		Q_ASSERT(self);
		emit self->wipeStarted(passes, bytesToWritePerPass);
	});

	KuuraSetPassStartedCallback(m_kuura, [](
		void* context,
		const char* path,
		uint16_t pass)
	{
		auto self = static_cast<KuuraRunner*>(context);
		Q_ASSERT(self);
		emit self->passStarted(path, pass);
	});

	KuuraSetProgressCallback(m_kuura, [](
		void* context,
		const char* path,
		uint16_t pass,
		uint64_t bytesWritten)
	{
		auto self = static_cast<KuuraRunner*>(context);
		Q_ASSERT(self);
		emit self->passProgressed(path, pass, bytesWritten);
		return self->m_keepRunning.load(); // TODO: investigate why isInterruptionRequested() does not work
	});

	KuuraSetPassCompletedCallback(m_kuura, [](
		void* context,
		const char* path,
		uint16_t pass)
	{
		auto self = static_cast<KuuraRunner*>(context);
		Q_ASSERT(self);
		emit self->passFinished(path, pass);
	});

	KuuraSetWipeCompletedCallback(m_kuura, [](
		void* context,
		uint16_t passes,
		uint64_t totalBytesWritten)
	{
		auto self = static_cast<KuuraRunner*>(context);
		Q_ASSERT(self);
		emit self->wipeCompleted(passes, totalBytesWritten);
	});

	KuuraSetErrorCallback(m_kuura, [](
		void* context,
		const char* path,
		KuuraStage stage,
		uint16_t pass,
		uint64_t bytesWritten,
		uint32_t errorCode)
	{
		auto self = static_cast<KuuraRunner*>(context);
		Q_ASSERT(self);
		emit self->errorOccurred(path, stage, pass, bytesWritten, errorCode);
	});
}

void KuuraRunner::run()
{
	qDebug() << "Started!";
	Q_ASSERT(m_kuura);

	m_keepRunning = true;

	attachCallbacks();

	if (!KuuraRun(m_kuura))
	{
		qDebug() << "Failed!";
		return;
	}

	qDebug() << (m_keepRunning ? "Finished!" : "Canceled.");
}
