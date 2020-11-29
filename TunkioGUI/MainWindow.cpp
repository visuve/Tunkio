#include "TunkioGUI-PCH.hpp"
#include "MainWindow.hpp"
#include "ProgressBarDelegate.hpp"
#include "DriveSelectDialog.hpp"
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
		case TunkioStage::Rewind:
			return "rewinding file pointer";
		case TunkioStage::Verify:
			return "verifying";
		case TunkioStage::Delete:
			return "deleting file";
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

	m_model = new WipePassModel(this);
	ui->tableViewWipePasses->setModel(m_model);
	ui->tableViewWipePasses->setItemDelegateForColumn(8, new ProgressBarDelegate(this));

	connect(ui->pushButtonAddPass, &QPushButton::clicked, this, &MainWindow::addPass);
	connect(ui->pushButtonRemovePass, &QPushButton::clicked, this, &MainWindow::removePass);
	connect(ui->pushButtonClearPasses, &QPushButton::clicked, this, &MainWindow::clearPasses);
	connect(ui->pushButtonStart, &QPushButton::clicked, this, &MainWindow::startWipe);

	connect(
		ui->tableViewWipePasses->selectionModel(),
		&QItemSelectionModel::selectionChanged,
		[this](const QItemSelection& selected, const QItemSelection&)
	{
		ui->pushButtonRemovePass->setEnabled(!selected.isEmpty());
	});

	const auto enableStartButton = [this]()
	{
		ui->pushButtonClearPasses->setEnabled(!m_model->isEmpty());
		ui->pushButtonStart->setEnabled(!m_model->isEmpty() && m_tunkio.get());
	};

	connect(m_model, &QAbstractItemModel::rowsInserted, enableStartButton);
	connect(m_model, &QAbstractItemModel::rowsRemoved, enableStartButton);
	// TODO: attach enableStartButton when m_tunkio is initialized

	m_textEditDialog = new TextEditorDialog(ui->lineEditFillValue);

	connect(ui->lineEditFillValue, &QLineEdit::textChanged, [&](const QString& text)
	{
		m_textEditDialog->setText(text);
	});

	connect(m_textEditDialog, &TextEditorDialog::accepted, [this]()
	{
		QString sentence = m_textEditDialog->text();
		ui->lineEditFillValue->setText(sentence);
	});

	ui->lineEditFillValue->installEventFilter(this);
}

MainWindow::~MainWindow()
{
	m_tunkio.reset();
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

bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::MouseButtonDblClick)
	{
		switch (ui->comboBoxFillType->currentIndex())
		{
			case 3:
			{
				m_textEditDialog->show();
				return true;
			}
			case 4:
			{
				QFileDialog dialog(this);
				dialog.setFileMode(QFileDialog::FileMode::ExistingFile);

				if (dialog.exec() == QFileDialog::Accepted)
				{
					const QString file = dialog.selectedFiles().first();
					ui->lineEditFillValue->setText(QDir::toNativeSeparators(file));
				}

				return true;
			}
		}
	}

	return QObject::eventFilter(watched, event);
}

void MainWindow::onFillTypeSelectionChanged(int index)
{
	switch (index)
	{
		case 0:
			ui->lineEditFillValue->setReadOnly(true);
			ui->lineEditFillValue->setInputMask("");
			ui->lineEditFillValue->setText("0x00");
			return;
		case 1:
			ui->lineEditFillValue->setReadOnly(true);
			ui->lineEditFillValue->setInputMask("");
			ui->lineEditFillValue->setText("0xff");
			return;
		case 2:
			ui->lineEditFillValue->setReadOnly(false);
			ui->lineEditFillValue->setInputMask("X");
			ui->lineEditFillValue->setText("T");
			return;
		case 3:
			ui->lineEditFillValue->setReadOnly(false);
			ui->lineEditFillValue->setInputMask("");
			ui->lineEditFillValue->setText(m_textEditDialog->text());
			return;
		case 4:
			ui->lineEditFillValue->setReadOnly(false);
			ui->lineEditFillValue->setInputMask("");
			ui->lineEditFillValue->setText("Please enter a path to a file here.");
			return;
		case 5:
			ui->lineEditFillValue->setReadOnly(true);
			ui->lineEditFillValue->setInputMask("");
			ui->lineEditFillValue->setText("MT19937 PRNG.");
			return;
	}

	qCritical() << index << " is out of bounds";
}

void MainWindow::addPass()
{
	int index = ui->comboBoxFillType->currentIndex();
	bool verify = ui->checkBoxVerify->isChecked();
	QString fill = ui->lineEditFillValue->text();

	switch (index)
	{
		case 0:
			return m_model->addPass(TunkioFillType::ZeroFill, fill, verify);
		case 1:
			return m_model->addPass(TunkioFillType::OneFill, fill, verify);
		case 2:
			return m_model->addPass(TunkioFillType::CharacterFill, fill, verify);
		case 3:
			return m_model->addPass(TunkioFillType::SentenceFill, fill, verify);
		case 4:
			return m_model->addPass(TunkioFillType::FileFill, fill, verify);
		case 5:
			return m_model->addPass(TunkioFillType::RandomFill, fill, verify);
		default:
			qCritical() << index << " is out of bounds";
			break;
	}
}

void MainWindow::removePass()
{
	Q_ASSERT(!m_model->isEmpty());

	for (const QModelIndex& selected : ui->tableViewWipePasses->selectionModel()->selectedRows())
	{
		m_model->removePass(selected.row());
	}
}

void MainWindow::clearPasses()
{
	Q_ASSERT(!m_model->isEmpty());

	ui->pushButtonClearPasses->setEnabled(false);
	ui->pushButtonStart->setEnabled(false);

	m_model->clearPasses();
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

void MainWindow::startWipe()
{
	Q_ASSERT(m_tunkio.get());
	Q_ASSERT(!m_tunkio->isRunning());
	Q_ASSERT(!m_model->isEmpty());

	m_model->clearStats();

	for (const WipePassModel::Pass& pass : m_model->passes())
	{
		if (!m_tunkio->addPass(pass.fillType, pass.fillValue, pass.verify))
		{
			QStringList message = { "Failed to add pass!" };
			message << QString("Pass type: %1").arg(Ui::fillTypeToString(pass.fillType));
			message << QString("Fill value: %1").arg(pass.fillValue.chopped(9));

			QMessageBox::critical(this, "Tunkio - An error occurred", message.join('\n'));
			return;
		}
	}

	ui->pushButtonStart->setEnabled(false);
	ui->pushButtonClearPasses->setEnabled(false);
	ui->pushButtonCancel->setEnabled(true);

	connect(m_tunkio.get(), &TunkioRunner::wipeStarted, m_model, &WipePassModel::onWipeStarted);
	connect(m_tunkio.get(), &TunkioRunner::passStarted, m_model, &WipePassModel::onPassStarted);
	connect(m_tunkio.get(), &TunkioRunner::passProgressed, m_model, &WipePassModel::onPassProgressed);
	connect(m_tunkio.get(), &TunkioRunner::passFinished, m_model, &WipePassModel::onPassFinished);
	connect(m_tunkio.get(), &TunkioRunner::wipeCompleted, m_model, &WipePassModel::onWipeCompleted);
	connect(m_tunkio.get(), &TunkioRunner::wipeCompleted, this, &MainWindow::onWipeCompleted);
	connect(m_tunkio.get(), &TunkioRunner::errorOccurred, this, &MainWindow::onError);

	connect(ui->pushButtonCancel, &QPushButton::clicked, m_tunkio.get(), &TunkioRunner::stop);

	m_tunkio->start();
}

void MainWindow::onError(TunkioStage stage, uint16_t pass, uint64_t bytesWritten, uint32_t errorCode)
{
	QStringList message = { QString("An error occurred while %1!\n").arg(toString(stage)) };
	message << QString("Pass: %1").arg(pass);
	message << QString("Bytes written: %1").arg(bytesWritten);
	message << QString("Operating system error code: %1").arg(errorCode);

	QMessageBox::critical(this, "Tunkio - An error occurred", message.join('\n'));

	ui->pushButtonStart->setEnabled(true);
	ui->pushButtonCancel->setEnabled(false);
}

void MainWindow::onWipeCompleted(uint16_t, uint64_t)
{
	ui->pushButtonStart->setEnabled(true);
	ui->pushButtonClearPasses->setEnabled(true);
	ui->pushButtonCancel->setEnabled(false);
}
