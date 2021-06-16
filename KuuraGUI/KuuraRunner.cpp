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

	wait();

	if (_kuura)
	{
		KuuraFree(_kuura);
		_kuura = nullptr;
	}

	qDebug() << "Destroyed.";
}

bool KuuraRunner::addTarget(KuuraTargetType type, const QString& path, bool remove)
{
	const std::filesystem::path native =
		QDir::toNativeSeparators(path).toStdString();

	return KuuraAddTarget(_kuura, type, native.c_str(), remove);
}

bool KuuraRunner::addTarget(KuuraTargetType type, const QFileInfo& path, bool remove)
{
	return addTarget(type, path.absoluteFilePath(), remove);
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
			return KuuraAddPass(_kuura, fillType, verify, nullptr);
		case KuuraFillType::ByteFill:
		{
			char character[] = { sequence[0], '\0' };
			return KuuraAddPass(_kuura, fillType, verify, character);
		}
		case KuuraFillType::SequenceFill:
		case KuuraFillType::FileFill:
			return KuuraAddPass(_kuura, fillType, verify, sequence.c_str());
	}

	return false;
}

bool KuuraRunner::keepRunning() const
{
	return QThread::currentThread()->isInterruptionRequested() == false;
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
		emit self->overwriteStarted(passes, bytesToWritePerPass);
	});

	KuuraSetTargetStartedCallback(_kuura, [](
		void* context,
		const KuuraChar* path,
		uint64_t bytesToWritePerPass)
	{
		auto self = static_cast<KuuraRunner*>(context);
		Q_ASSERT(self);
		self->targetStarted(path, bytesToWritePerPass);
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
		return self->keepRunning();
	});

	KuuraSetPassCompletedCallback(_kuura, [](
		void* context,
		const KuuraChar* path,
		uint16_t pass)
	{
		auto self = static_cast<KuuraRunner*>(context);
		Q_ASSERT(self);
		emit self->passCompleted(path, pass);
	});

	KuuraSetTargetCompletedCallback(_kuura, [](
		void* context,
		const KuuraChar* path,
		uint64_t bytesWritten)
	{
		auto self = static_cast<KuuraRunner*>(context);
		Q_ASSERT(self);
		emit self->targetCompleted(path, bytesWritten);
	});

	KuuraSetOverwriteCompletedCallback(_kuura, [](
		void* context,
		uint16_t passes,
		uint64_t totalBytesWritten)
	{
		auto self = static_cast<KuuraRunner*>(context);
		Q_ASSERT(self);
		emit self->overwriteCompleted(passes, totalBytesWritten);
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

	attachCallbacks();

	if (!KuuraRun(_kuura))
	{
		qDebug() << "Failed!";
		return;
	}

	qDebug() << "Finished!";
}
