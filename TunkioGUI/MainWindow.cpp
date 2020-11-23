#include "TunkioGUI-PCH.hpp"
#include "MainWindow.hpp"
#include "ProgressBarDelegate.hpp"
#include "DriveSelectDialog.hpp"
#include "TextEditorDialog.hpp"
#include "ui_MainWindow.h"

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

	connect(
		ui->comboBoxFillType,
		static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
		this,
		&MainWindow::onFillTypeSelectionChanged);

	ui->lineEditFillValue->installEventFilter(new TextEditorDialog());

	m_model = new WipePassModel(this);
	ui->tableViewWipePasses->setModel(m_model);
	ui->tableViewWipePasses->setItemDelegateForColumn(7, new ProgressBarDelegate(this));

	connect(ui->pushButtonAddPass, &QPushButton::clicked, this, &MainWindow::addPass);
	connect(ui->pushButtonStart, &QPushButton::clicked, this, &MainWindow::startWipe);
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
		m_tunkio = std::make_shared<TunkioRunner>(file, TunkioTargetType::FileWipe);
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
		m_tunkio = std::make_shared<TunkioRunner>(directory, TunkioTargetType::DirectoryWipe);
	}
}

void MainWindow::onOpenDriveDialog()
{
	DriveSelectDialog dialog(this);

	if (dialog.exec() == QDialog::Accepted)
	{
		const QString drive = dialog.selectedDrive();
		ui->lineEditSelectedPath->setText(drive);
		m_tunkio = std::make_shared<TunkioRunner>(drive, TunkioTargetType::DriveWipe);
	}
}

void MainWindow::onFillTypeSelectionChanged(int index)
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
			ui->lineEditFillValue->setEnabled(true);
			ui->lineEditFillValue->setText("?");
			return;
		case 3:
			ui->lineEditFillValue->setEnabled(true);
			ui->lineEditFillValue->setText("Please enter a sentence here.");
			return;
		case 4:
			ui->lineEditFillValue->setEnabled(true);
			ui->lineEditFillValue->setText("Please enter a path to a file here.");
			return;
		case 5:
			ui->lineEditFillValue->setEnabled(false);
			ui->lineEditFillValue->setText("A PRNG engine will be used.");
			return;
	}

	qCritical() << index << " is out of bounds";
}

void MainWindow::addPass()
{
	Q_ASSERT(m_tunkio.get());

	int index = ui->comboBoxFillType->currentIndex();
	bool verify = ui->checkBoxVerify->isChecked();
	QString fill = ui->lineEditFillValue->text();

	switch (index)
	{
		// TODO: clean up this temporary mess and change the functionality so,
		// that passes can be added to the GUI at any time, but they are added to Tunkio
		// just before start
		case 0:
			if (m_tunkio->addPass(TunkioFillType::ZeroFill, QString(), verify))
			{
				m_model->onPassAdded(TunkioFillType::ZeroFill, "0x00", verify);
			}
			else
			{
				qWarning() << "Failed to add pass";
			}
			return;
		case 1:
			if (m_tunkio->addPass(TunkioFillType::OneFill, QString(), verify))
			{
				m_model->onPassAdded(TunkioFillType::OneFill, "0xFF", verify);
			}
			else
			{
				qWarning() << "Failed to add pass";
			}
			return;
		case 2:
			if (m_tunkio->addPass(TunkioFillType::CharacterFill, fill, verify))
			{
				m_model->onPassAdded(TunkioFillType::CharacterFill, fill.at(0), verify);
			}
			else
			{
				qWarning() << "Failed to add pass";
			}
			return;
		case 3:
			if (m_tunkio->addPass(TunkioFillType::SentenceFill, fill, verify))
			{
				m_model->onPassAdded(TunkioFillType::SentenceFill, fill, verify);
			}
			else
			{
				qWarning() << "Failed to add pass";
			}
			return;
		case 4:
			if (m_tunkio->addPass(TunkioFillType::FileFill, fill, verify))
			{
				m_model->onPassAdded(TunkioFillType::FileFill, fill, verify);
			}
			else
			{
				qWarning() << "Failed to add pass";
			}
			return;
		case 5:
			if (m_tunkio->addPass(TunkioFillType::RandomFill, QString(), verify))
			{
				m_model->onPassAdded(TunkioFillType::RandomFill, "Random", verify);
			}
			else
			{
				qWarning() << "Failed to add pass";
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

void MainWindow::onError(TunkioStage stage, uint16_t pass, uint64_t bytesWritten, uint32_t errorCode)
{
	QStringList message = { QString("An error occurred while %1!\n").arg(toString(stage)) };
	message << QString("Pass: %1").arg(pass);
	message << QString("Bytes written: %1").arg(bytesWritten);
	message << QString("Operating system error code: %1").arg(errorCode);

	QMessageBox::critical(this, "Tunkio - An error occurred", message.join('\n'));
}

void MainWindow::startWipe()
{
	Q_ASSERT(m_tunkio.get());

	connect(m_tunkio.get(), &TunkioRunner::wipeStarted, m_model, &WipePassModel::onWipeStarted);
	connect(m_tunkio.get(), &TunkioRunner::passStarted, m_model, &WipePassModel::onPassStarted);
	connect(m_tunkio.get(), &TunkioRunner::passProgressed, m_model, &WipePassModel::onPassProgressed);
	connect(m_tunkio.get(), &TunkioRunner::passFinished, m_model, &WipePassModel::onPassFinished);
	connect(m_tunkio.get(), &TunkioRunner::wipeCompleted, m_model, &WipePassModel::onWipeCompleted);
	connect(m_tunkio.get(), &TunkioRunner::errorOccurred, this, &MainWindow::onError);

	m_tunkio->start();
}
