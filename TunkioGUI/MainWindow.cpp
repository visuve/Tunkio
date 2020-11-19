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
		Q_ASSERT(m_tunkio.get());
		Q_ASSERT(m_model);

		// connect(m_tunkio.get(), &QThread::finished, m_tunkio.get(), &QObject::deleteLater);

		// I do not know what the fuck is going on, i.e. why the connections work _ONLY_ like this
		connect(m_tunkio.get(), &TunkioRunner::wipeStarted, [this](uint16_t totalIterations, uint64_t bytesToWritePerIteration)
		{
			qDebug() << "wipeStarted";
			m_model->onWipeStarted(totalIterations, bytesToWritePerIteration);
		});

		connect(m_tunkio.get(), &TunkioRunner::passStarted, [this](uint16_t pass)
		{
			qDebug() << "passStarted";
			m_model->onPassStarted(pass);
		});

		connect(m_tunkio.get(), &TunkioRunner::passProgressed, [this](uint16_t pass, uint64_t bytesWritten)
		{
			qDebug() << "passProgressed";
			m_model->onPassProgressed(pass, bytesWritten);
		});

		connect(m_tunkio.get(), &TunkioRunner::passFinished, [this](uint16_t pass)
		{
			qDebug() << "passFinished";
			m_model->onPassFinished(pass);
		});

		connect(m_tunkio.get(), &TunkioRunner::wipeCompleted, [this](uint16_t passes, uint64_t totalBytesWritten)
		{
			qDebug() << "wipeCompleted";
			m_model->onWipeCompleted(passes, totalBytesWritten);
		});

		// connect(m_tunkio.get(), &TunkioRunner::wipeStarted, m_model, &WipePassModel::onWipeStarted);
		// connect(m_tunkio.get(), &TunkioRunner::passStarted, m_model, &WipePassModel::onPassStarted);
		// connect(m_tunkio.get(), &TunkioRunner::passProgressed, m_model, &WipePassModel::onPassProgressed);
		// connect(m_tunkio.get(), &TunkioRunner::passFinished, m_model, &WipePassModel::onPassFinished);
		// connect(m_tunkio.get(), &TunkioRunner::wipeCompleted, m_model, &WipePassModel::onWipeCompleted);

		connect(m_tunkio.get(), &TunkioRunner::errorOccurred, [this](TunkioStage stage, uint16_t currentIteration, uint64_t bytesWritten, uint32_t errorCode)
		{
			onError(stage, currentIteration, bytesWritten, errorCode);
		});

		m_tunkio->start();
	});
}

MainWindow::~MainWindow()
{
	m_tunkio = nullptr;
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
		m_tunkio = std::make_shared<TunkioRunner>(file, TunkioTargetType::File);
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
		m_tunkio = std::make_shared<TunkioRunner>(directory, TunkioTargetType::Directory);
	}
}

void MainWindow::onOpenDriveDialog()
{
	DriveSelectDialog dialog(this);

	if (dialog.exec() == QDialog::Accepted)
	{
		const QString drive = dialog.selectedDrive();
		ui->lineEditSelectedPath->setText(drive);
		m_tunkio = std::make_shared<TunkioRunner>(drive, TunkioTargetType::Drive);
	}
}

void MainWindow::addPass()
{
	Q_ASSERT(m_tunkio.get());

	int index = ui->comboBoxStepType->currentIndex();
	bool verify = ui->checkBoxVerify->isChecked();
	QString fill = ui->lineEditFillValue->text();

	switch (index)
	{
		case 0:
			if (m_tunkio->addPass(TunkioFillType::Zeroes, QString(), verify))
			{
				m_model->onPassAdded(TunkioFillType::Zeroes, "0x00", verify);
			}

			return;
		case 1:
			if (m_tunkio->addPass(TunkioFillType::Ones, QString(), verify))
			{
				m_model->onPassAdded(TunkioFillType::Ones, "0xFF", verify);
			}
			return;
		case 2:
			if (m_tunkio->addPass(TunkioFillType::Random, QString(), verify))
			{
				m_model->onPassAdded(TunkioFillType::Random, "Random", verify);
			}
			return;
		case 3:
			if (m_tunkio->addPass(TunkioFillType::Character, fill, verify))
			{
				m_model->onPassAdded(TunkioFillType::Character, fill.at(0), verify);
			}
			return;
		case 4:
			if (m_tunkio->addPass(TunkioFillType::Character, fill, verify))
			{
				m_model->onPassAdded(TunkioFillType::Sentence, fill, verify);
			}
			return;
	}

	qCritical() << index << " is out of bounds";
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

void MainWindow::onError(TunkioStage stage, uint16_t currentIteration, uint64_t bytesWritten, uint32_t errorCode)
{
	QStringList message = { QString("An error occurred while %1!\n").arg(toString(stage)) };
	message << QString("Pass: %1").arg(currentIteration);
	message << QString("Bytes written: %1").arg(bytesWritten);
	message << QString("Operating system error code: %1").arg(errorCode);

	QMessageBox::critical(this, "Tunkio - An error occurred", message.join('\n'));
}
