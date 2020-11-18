#include "MainWindow.hpp"
#include "ProgressBarDelegate.hpp"
#include "DriveSelectDialog.hpp"
#include "ui_MainWindow.h"

#include <QDebug>
#include <QTreeWidgetItem>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QTime>
#include <QMapIterator>
#include <QDir>
#include <QActionGroup>
#include <QInputDialog>
#include <QDir>

QString toString(TunkioStage type)
{
	switch (type)
	{
		case TunkioStage::Open:
			return "opening";
		case TunkioStage::Size:
			return "querying size";
		case TunkioStage::Write:
			return "writing";
		case TunkioStage::Verify:
			return "verifying";
		case TunkioStage::Remove:
			return "removing";
	}

	qCritical() << int(type) << " is out of bounds";
	return "Unknown?";
}

MainWindow::MainWindow(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow())
{
	ui->setupUi(this);

	ui->actionOpenFile->setIcon(QApplication::style()->standardIcon(QStyle::SP_FileIcon));
	connect(ui->actionOpenFile, &QAction::triggered, this, &MainWindow::onOpenFileDialog);

	ui->actionOpenDirectory->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon));
	connect(ui->actionOpenDirectory, &QAction::triggered, this, &MainWindow::onOpenDirectoryDialog);

	ui->actionOpenDrive->setIcon(QApplication::style()->standardIcon(QStyle::SP_DriveHDIcon));
	connect(ui->actionOpenDrive, &QAction::triggered, this, &MainWindow::onOpenDriveDialog);

	ui->actionExit->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton));
	connect(ui->actionExit, &QAction::triggered, qApp, &QApplication::quit);

	ui->actionAbout->setIcon(QApplication::style()->standardIcon(QStyle::SP_MessageBoxInformation));
	connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAbout);

	ui->actionLicenses->setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarMenuButton));
	connect(ui->actionLicenses, &QAction::triggered, [this]()
	{
		QMessageBox::aboutQt(this, "Tunkio");
	});

	connect(ui->comboBoxStepType, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this](int index)
	{
		switch (index)
		{
			case 0:
				ui->lineEditFillValue->setEnabled(false);
				ui->lineEditFillValue->setText("0x00");
				return;
			case 1:
				ui->lineEditFillValue->setEnabled(false);
				ui->lineEditFillValue->setText("0xFF");
				return;
			case 2:
				ui->lineEditFillValue->setEnabled(false);
				ui->lineEditFillValue->setText("A PRNG engine will be used.");
				return;
			case 3:
				ui->lineEditFillValue->setEnabled(true);
				ui->lineEditFillValue->setText("?");
				return;
			case 4:
				ui->lineEditFillValue->setEnabled(true);
				ui->lineEditFillValue->setText("Please enter a sentence here.");
				return;
		}
	});

	m_model = new WipePassModel(this);
	ui->tableViewWipePasses->setModel(m_model);
	ui->tableViewWipePasses->setItemDelegateForColumn(7, new ProgressBarDelegate(this));

	connect(ui->pushButtonAddStep, &QPushButton::clicked, this, &MainWindow::addPass);

	connect(ui->pushButtonStart, &QPushButton::clicked, [this]()
	{
		TunkioHandle* handle = m_tunkio.Get();
		Q_ASSERT(handle != nullptr);
		attachCallbacks();
		TunkioRun(handle);
		qDebug() << "FOO";
	});
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::onOpenFileDialog()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::FileMode::ExistingFile);

	if (dialog.exec() == QFileDialog::Accepted)
	{
		const QString file = dialog.selectedFiles().first();
		ui->lineEditSelectedPath->setText(QDir::toNativeSeparators(file));
		m_tunkio.Reset(
			TunkioInitialize(m_model, file.toUtf8().constData(), TunkioTargetType::File));
	}
}

void MainWindow::onOpenDirectoryDialog()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::FileMode::Directory);
	dialog.setOption(QFileDialog::ShowDirsOnly, true);

	if (dialog.exec() == QFileDialog::Accepted)
	{
		const QString directory = dialog.selectedFiles().first();
		ui->lineEditSelectedPath->setText(QDir::toNativeSeparators(directory));
		m_tunkio.Reset(
			TunkioInitialize(m_model, directory.toUtf8().constData(), TunkioTargetType::Directory));
	}
}

void MainWindow::onOpenDriveDialog()
{
	DriveSelectDialog dialog(this);

	if (dialog.exec() == QDialog::Accepted)
	{
		const QString drive = dialog.selectedDrive();
		ui->lineEditSelectedPath->setText(drive);
		m_tunkio.Reset(
			TunkioInitialize(m_model, drive.toUtf8().constData(), TunkioTargetType::Drive));
	}
}

void MainWindow::addPass()
{

	Q_ASSERT(m_tunkio);

	int index = ui->comboBoxStepType->currentIndex();
	bool verify = ui->checkBoxVerify->isChecked();
	QString fill = ui->lineEditFillValue->text();
	const char* character = &fill.toStdString()[0];
	const char* sentence = fill.toStdString().c_str();


	switch (index)
	{
		case 0:
			if (TunkioAddWipeRound(m_tunkio, TunkioFillType::Zeroes, verify, nullptr))
			{
				m_model->addPass(TunkioFillType::Zeroes, "0x00", verify);
			}
			return;
		case 1:
			if (TunkioAddWipeRound(m_tunkio, TunkioFillType::Ones, verify, nullptr))
			{
				m_model->addPass(TunkioFillType::Ones, "0xFF", verify);
			}
			return;
		case 2:
			if (TunkioAddWipeRound(m_tunkio, TunkioFillType::Random, verify, nullptr))
			{
				m_model->addPass(TunkioFillType::Random, "Random", verify);
			}
			return;
		case 3:
			if (TunkioAddWipeRound(m_tunkio, TunkioFillType::Character, verify, character))
			{
				m_model->addPass(TunkioFillType::Character, fill.at(0), verify);
			}
			return;
		case 4:
			if (TunkioAddWipeRound(m_tunkio, TunkioFillType::Character, verify, sentence))
			{
				m_model->addPass(TunkioFillType::Sentence, fill, verify);
			}
			return;
	}

	qCritical() << index << " is out of bounds";
}

void MainWindow::attachCallbacks()
{
	TunkioStartedCallback* started = [](
		void* context,
		uint16_t totalIterations,
		uint64_t bytesToWritePerIteration)
	{
		qDebug() << "Wipe started: " << totalIterations << '/' << bytesToWritePerIteration;
		auto model = static_cast<WipePassModel*>(context);
		model->wipeStarted(totalIterations, bytesToWritePerIteration);
	};

	TunkioIterationStartedCallback* iterationStarted = [](
		void* context,
		uint16_t currentIteration)
	{
		qDebug() << "Iteration started: " << currentIteration;
		auto model = static_cast<WipePassModel*>(context);
		model->setPassStarted(currentIteration);
	};

	TunkioProgressCallback* progress = [](
		void* context,
		uint16_t currentIteration,
		uint64_t bytesWritten)
	{
		qDebug() << "Progress: " << currentIteration << '/' << bytesWritten;
		auto model = static_cast<WipePassModel*>(context);
		model->setPassProgress(currentIteration, bytesWritten);
		return true;
	};

	TunkioErrorCallback* error = [](
		void* context,
		TunkioStage stage,
		uint16_t currentIteration,
		uint64_t bytesWritten,
		uint32_t errorCode)
	{
		auto model = static_cast<WipePassModel*>(context);
		auto self = dynamic_cast<MainWindow*>(model->parent()->parent());

		QStringList message = { QString("An error occurred while %1!\n").arg(toString(stage)) };
		message << QString("Pass: %1").arg(currentIteration);
		message << QString("Bytes written: %1").arg(bytesWritten);
		message << QString("Operating system error code: %1").arg(errorCode);

		QMessageBox::critical(self, "Tunkio - An error occurred", message.join('\n'));
	};

	TunkioIterationCompletedCallback* iterationCompleted = [](
		void* context,
		uint16_t currentIteration)
	{
		qDebug() << "Completed iteration: " << currentIteration;
		auto model = static_cast<WipePassModel*>(context);
		model->setPassFinished(currentIteration);
	};

	TunkioCompletedCallback* completed = [](
		void* context,
		uint16_t totalIterations,
		uint64_t totalBytesWritten)
	{
		qDebug() << "Completed Wipe: " << totalIterations << '/' << totalBytesWritten;
		auto model = static_cast<WipePassModel*>(context);
		model->wipeCompleted(totalIterations, totalBytesWritten);
	};

	TunkioSetStartedCallback(m_tunkio, started);
	TunkioSetIterationStartedCallback(m_tunkio, iterationStarted);
	TunkioSetProgressCallback(m_tunkio, progress);
	TunkioSetErrorCallback(m_tunkio, error);
	TunkioSetIterationCompletedCallback(m_tunkio, iterationCompleted);
	TunkioSetCompletedCallback(m_tunkio, completed);
}

void MainWindow::onAbout()
{
	QStringList text;
	text << "Tunkio - Data Erasure Tool version 0.1.";
	text << "";
	text << "Tunkio is yet another data erasure tool.";
	text << "";
	text << "With Tunkio, you can add passes to wipe the data according to your will,"
		"hence fullfilling even the most obscure standards";
	text << "";
	text << "Tunkio is licenced under GPLv2.";
	text << "";
	text << "This GUI uses Qt version " + QString(qVersion()).append('.');
	text << "See 'About Qt' for more details.";
	text << "";

	QMessageBox::about(this, "Tunkio", text.join('\n'));
}