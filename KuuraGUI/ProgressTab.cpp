#include "KuuraGUI-PCH.hpp"
#include "ProgressTab.hpp"
#include "ProgressBarDelegate.hpp"
#include "ProgressModel.hpp"
#include "ui_ProgressTab.h"

ProgressTab::ProgressTab(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ProgressTab),
	_model(new ProgressModel(
		{
			"Path",				// 0
			"Bytes Written",	// 1
			"Bytes Left",		// 2
			"Time Taken",		// 3
			"Time Left",		// 4
			"Speed",			// 5
			"Progress"			// 6
		}, this))
{
	ui->setupUi(this);

	ui->treeViewProgress->setModel(_model);
	ui->treeViewProgress->setItemDelegateForColumn(6, new ProgressBarDelegate(this));

	connect(ui->pushButtonNext, &QPushButton::clicked, [this]
	{
		emit nextRequested();
	});

	connect(&_totalProgressUpdater, &QTimer::timeout, [this]()
	{
		updateTotalProgress();
	});

	_totalProgressUpdater.setInterval(100);
	ui->progressBar->setMinimum(0);
	ui->progressBar->setMaximum(100);
}

ProgressTab::~ProgressTab()
{
	delete ui;
	qDebug();
}

void ProgressTab::addTarget(const QString& path)
{
	_model->addTarget(path);
}

void ProgressTab::addTarget(const QFileInfo& path)
{
	addTarget(QDir::toNativeSeparators(path.absoluteFilePath()));
}

void ProgressTab::addPass(const QVariantList& pass)
{
	_model->addPass(pass);
}

void ProgressTab::onOverwriteStarted(uint16_t passes, uint64_t bytesToWritePerPass)
{
	qDebug() << "Wipe started:" << passes << '/' << bytesToWritePerPass;

	_totalProgress.reset(bytesToWritePerPass * passes);
	ui->progressBar->setValue(0);
	ui->pushButtonNext->setEnabled(false);
	_totalProgressUpdater.start();
}

void ProgressTab::onTargetStarted(const std::filesystem::path& path, uint64_t bytesToWrite)
{
	qDebug() << "Target started:" << Ui::toQString(path) << '/' << bytesToWrite;

	//_model->setData(QString::fromStdWString(path), -1, 2, bytesToWrite);
}

void ProgressTab::onPassStarted(const std::filesystem::path& path, uint16_t pass)
{
	qDebug() << "Pass started:" << Ui::toQString(path) << '/' << pass;
}

void ProgressTab::onPassProgressed(const std::filesystem::path&, uint16_t, uint64_t bytesWritten)
{
	// qDebug() << "Pass progressed:" << Ui::toQString(path) << '/' << pass << '/' << bytesWritten;

	_totalProgress.update(bytesWritten);
}

void ProgressTab::onPassCompleted(const std::filesystem::path& path, uint16_t pass)
{
	qDebug() << "Pass completed:" << Ui::toQString(path) << '/' << pass;
}

void ProgressTab::onTargetCompleted(const std::filesystem::path& path, uint64_t bytesWritten)
{
	qDebug() << "Target completed:" << Ui::toQString(path) << '/' << bytesWritten;

	//_model->setData(QString::fromStdWString(path), -1, 1, float(bytesWritten));
}

void ProgressTab::onOverwriteCompleted(uint16_t passes, uint64_t totalBytesWritten)
{
	qDebug() << "Wipe completed:" << passes << '/' << totalBytesWritten;

	_totalProgressUpdater.stop();
	_totalProgress.complete();
	updateTotalProgress();
	ui->progressBar->setValue(100);
	ui->pushButtonNext->setEnabled(true);
}

void ProgressTab::onError()
{
	_totalProgressUpdater.stop();
}

void ProgressTab::updatePassProgress()
{
	// TODO: implement
}

void ProgressTab::updateTotalProgress()
{
	qDebug();

	{
		const float milliSecondsTaken = _totalProgress.milliSecondsTaken();
		const QString timeTaken = Ui::millisecondsToString(milliSecondsTaken);
		ui->labelTimeTaken->setText(timeTaken);
	}
	{
		const float bytesPerSecond = _totalProgress.bytesPerMillisecond() * 1000.0f;
		const QString speed = Ui::bytesToString(bytesPerSecond).append("/s");
		ui->labelSpeed->setText(speed);
	}
	{
		const float milliSecondsLeft = _totalProgress.millisecondsLeft();
		const QString timeLeft = Ui::millisecondsToString(milliSecondsLeft);
		ui->labelTimeLeft->setText(timeLeft);
	}
	{
		const int percentage = static_cast<int>(_totalProgress.percentageDone());
		ui->progressBar->setValue(percentage);
	}
}
