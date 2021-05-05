#include "KuuraGUI-PCH.hpp"
#include "ProgressTab.hpp"
#include "ProgressBarDelegate.hpp"
#include "ProgressModel.hpp"
#include "ui_ProgressTab.h"

ProgressTab::ProgressTab(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ProgressTab),
	_model(new ProgressModel(this))
{
	ui->setupUi(this);

	ui->treeViewProgress->setModel(_model);
	ui->treeViewProgress->setItemDelegateForColumn(6, new ProgressBarDelegate(this));

	connect(ui->pushButtonNext, &QPushButton::clicked, [this]
	{
		emit nextRequested();
	});
}

ProgressTab::~ProgressTab()
{
	delete ui;
	qDebug();
}

void ProgressTab::addTarget(const std::filesystem::path& path)
{
	_model->addNode(path);
}

void ProgressTab::onOverwriteStarted(uint16_t passes, uint64_t bytesToWritePerPass)
{
	_bytesToProcess = float(bytesToWritePerPass) * float(passes);
	_overwriteStartTime = std::chrono::steady_clock::now();
	ui->progressBar->setValue(0);
	ui->pushButtonNext->setEnabled(false);

	_model->setPassCount(passes);

	qDebug() << "Wipe started:" << passes << '/' << bytesToWritePerPass;
}

void ProgressTab::onTargetStarted(const std::filesystem::path& path, uint64_t bytesToWrite)
{
	_model->beginTarget(path, bytesToWrite);
}

void ProgressTab::onPassStarted(const std::filesystem::path& path, uint16_t pass)
{
	qDebug() << "Pass started:" << Ui::toQString(path) << '/' << pass;
}

void ProgressTab::onPassProgressed(const std::filesystem::path& path, uint16_t pass, uint64_t bytesWritten)
{
	updateTotalProgress();

	_model->updateNode(path, pass, bytesWritten);
}

void ProgressTab::onPassCompleted(const std::filesystem::path& path, uint16_t pass)
{
	qDebug() << "Pass completed:" << Ui::toQString(path) << '/' << pass;
}

void ProgressTab::onTargetCompleted(const std::filesystem::path& path, uint64_t bytesWritten)
{
	qDebug() << "Target completed:" << Ui::toQString(path) << '/' << bytesWritten;
}

void ProgressTab::onOverwriteCompleted(uint16_t passes, uint64_t totalBytesWritten)
{
	qDebug() << "Wipe completed:" << passes << '/' << totalBytesWritten;

	updateTotalProgress();
	ui->progressBar->setValue(100);
	ui->pushButtonNext->setEnabled(true);
}

void ProgressTab::updateTotalProgress()
{
	const float bytesWrittenTotal = _model->bytesWrittenTotal();

	const std::chrono::steady_clock::time_point time = std::chrono::steady_clock::now();
	const float milliSecondsTaken = std::chrono::duration_cast<Ui::MilliSeconds>(time - _overwriteStartTime).count();

	if (bytesWrittenTotal <= 0.00f || milliSecondsTaken <= 0.00f)
	{
		return;
	}

	const float bytesPerMilliSecond = bytesWrittenTotal / milliSecondsTaken;

	{
		const QString timeTaken = Ui::millisecondsToString(milliSecondsTaken);
		ui->labelTimeTaken->setText(timeTaken);
	}
	{
		const float bytesPerSecond = bytesPerMilliSecond * 1000.0f;
		const QString speed = Ui::bytesToString(bytesPerSecond).append("/s");
		ui->labelSpeed->setText(speed);
	}
	{
		const float bytesLeft = _bytesToProcess - bytesWrittenTotal;
		const float milliSecondsLeft = bytesLeft / bytesPerMilliSecond;
		const QString timeLeft = Ui::millisecondsToString(milliSecondsLeft);
		ui->labelTimeLeft->setText(timeLeft);
	}
	{
		const float percentage = (bytesWrittenTotal / _bytesToProcess) * 100.0f;
		ui->progressBar->setValue(static_cast<int>(percentage));
	}
}
