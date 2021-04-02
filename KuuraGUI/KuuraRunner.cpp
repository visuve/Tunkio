#include "KuuraGUI-PCH.hpp"
#include "KuuraRunner.hpp"

KuuraRunner::KuuraRunner(QObject* parent) :
	QThread(parent),
	_kuura(KuuraInitialize(this))
{
}

KuuraRunner::~KuuraRunner()
{
	qDebug() << "Destroying...";

	_keepRunning = false;
	wait();

	if (_kuura)
	{
		KuuraFree(_kuura);
	}

	qDebug() << "Destroyed.";
}

bool KuuraRunner::addTarget(const std::filesystem::path& path, KuuraTargetType type, bool remove)
{
	return KuuraAddTarget(_kuura, path.c_str(), type, remove);
}

bool KuuraRunner::addPass(KuuraFillType fillType, const QByteArray& fillValue, bool verify)
{
	Q_ASSERT(_kuura);

	const std::string sequence = fillValue.toStdString();

	switch (fillType)
	{
		case KuuraFillType::ZeroFill:
		case KuuraFillType::OneFill:
		case KuuraFillType::RandomFill:
			return KuuraAddOverwriteRound(_kuura, fillType, verify, nullptr);
		case KuuraFillType::ByteFill:
		{
			char character[] = { sequence[0], '\0' };
			return KuuraAddOverwriteRound(_kuura, fillType, verify, character);
		}
		case KuuraFillType::SequenceFill:
		case KuuraFillType::FileFill:
			return KuuraAddOverwriteRound(_kuura, fillType, verify, sequence.c_str());
	}

	return false;
}

void KuuraRunner::stop()
{
	_keepRunning = false;
}

void KuuraRunner::attachCallbacks()
{
	if (!_kuura)
	{
		qCritical() << "Kuura not initialized properly!";
		return;
	}

	KuuraSetOverwriteStartedCallback(_kuura, [](
		void* context,
		uint16_t passes,
		uint64_t bytesToWritePerPass)
	{
		auto self = static_cast<KuuraRunner*>(context);
		Q_ASSERT(self);
		emit self->wipeStarted(passes, bytesToWritePerPass);
	});

	KuuraSetPassStartedCallback(_kuura, [](
		void* context,
		const KuuraChar* path,
		uint16_t pass)
	{
		auto self = static_cast<KuuraRunner*>(context);
		Q_ASSERT(self);
		emit self->passStarted(path, pass);
	});

	KuuraSetProgressCallback(_kuura, [](
		void* context,
		const KuuraChar* path,
		uint16_t pass,
		uint64_t bytesWritten)
	{
		auto self = static_cast<KuuraRunner*>(context);
		Q_ASSERT(self);
		emit self->passProgressed(path, pass, bytesWritten);
		return self->_keepRunning.load(); // TODO: investigate why isInterruptionRequested() does not work
	});

	KuuraSetPassCompletedCallback(_kuura, [](
		void* context,
		const KuuraChar* path,
		uint16_t pass)
	{
		auto self = static_cast<KuuraRunner*>(context);
		Q_ASSERT(self);
		emit self->passFinished(path, pass);
	});

	KuuraSetOverwriteCompletedCallback(_kuura, [](
		void* context,
		uint16_t passes,
		uint64_t totalBytesWritten)
	{
		auto self = static_cast<KuuraRunner*>(context);
		Q_ASSERT(self);
		emit self->wipeCompleted(passes, totalBytesWritten);
	});

	KuuraSetErrorCallback(_kuura, [](
		void* context,
		const KuuraChar* path,
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
	Q_ASSERT(_kuura);

	_keepRunning = true;

	attachCallbacks();

	if (!KuuraRun(_kuura))
	{
		qDebug() << "Failed!";
		return;
	}

	qDebug() << (_keepRunning ? "Finished!" : "Canceled.");
}
