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
		case TunkioStage::Unmount:
			return "unmounting";
		case TunkioStage::Size:
			return "querying size";
		case TunkioStage::Write:
			return "writing";
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

	// ui->menuFile->setIcon(QApplication::style()->standardIcon(QStyle::SP_ComputerIcon));
	// ui->menuPresets->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaPlay));

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

	connect(ui->actionNIST_SP_800_88_Rev_1, &QAction::triggered, [this]()
	{
		ui->groupBoxAddPass->setEnabled(false);
		m_model->clearPasses();
		m_model->addPass(TunkioFillType::ZeroFill, false);
	});

	connect(ui->actionBritish_HMG_Infosec_Standard_5_Baseline_Standard, &QAction::triggered, [this]()
	{
		ui->groupBoxAddPass->setEnabled(false);
		m_model->clearPasses();
		m_model->addPass(TunkioFillType::RandomFill, true);
	});

	connect(ui->actionBritish_HMG_Infosec_Standard_5_Enhanced_Standard, &QAction::triggered, [this]()
	{
		ui->groupBoxAddPass->setEnabled(false);
		m_model->clearPasses();
		m_model->addPass(TunkioFillType::OneFill, false);
		m_model->addPass(TunkioFillType::ZeroFill, false);
		m_model->addPass(TunkioFillType::RandomFill, true);
	});

	connect(ui->actionCommunications_Security_Establishment_Canada_ITSG_06, &QAction::triggered, [this]()
	{
		ui->groupBoxAddPass->setEnabled(false);
		m_model->clearPasses();
		m_model->addPass(TunkioFillType::ZeroFill, false);
		m_model->addPass(TunkioFillType::OneFill, false);
		m_model->addPass(TunkioFillType::RandomFill, false);
	});

	connect(ui->actionGerman_Federal_Office_for_Information_Security, &QAction::triggered, [this]()
	{
		QMessageBox::critical(
			this,
			"Tunkio - Not implemented yet!",
			"This feature is not yet implemented");
	});

	connect(ui->actionU_S_DoD_Unclassified_Computer_Hard_Drive_Disposition, &QAction::triggered, [this]()
	{
		ui->groupBoxAddPass->setEnabled(false);
		m_model->clearPasses();
		m_model->addPass(TunkioFillType::CharacterFill, false, "\x55");
		m_model->addPass(TunkioFillType::CharacterFill, false, "\xAA");
		m_model->addPass(TunkioFillType::RandomFill,  false);
	});

	connect(ui->actionBruce_Schneier_s_Algorithm, &QAction::triggered, [this]()
	{
		ui->groupBoxAddPass->setEnabled(false);
		m_model->clearPasses();
		m_model->addPass(TunkioFillType::OneFill, false);
		m_model->addPass(TunkioFillType::ZeroFill, false);
		m_model->addPass(TunkioFillType::RandomFill, false);
		m_model->addPass(TunkioFillType::RandomFill, false);
		m_model->addPass(TunkioFillType::RandomFill, false);
		m_model->addPass(TunkioFillType::RandomFill, false);
		m_model->addPass(TunkioFillType::RandomFill, false);
	});

	connect(ui->actionPeter_Gutmann_s_Algorithm, &QAction::triggered, [this]()
	{
		ui->groupBoxAddPass->setEnabled(false);
		thread_local std::random_device device;
		thread_local std::mt19937_64 engine(device());

		std::array<std::pair<TunkioFillType, QByteArray>, 35> gutmannsRecipe =
		{
			std::make_pair(TunkioFillType::RandomFill, QByteArray()),
			{ TunkioFillType::RandomFill, {} },
			{ TunkioFillType::RandomFill, {} },
			{ TunkioFillType::RandomFill, {} },

			{ TunkioFillType::CharacterFill, "\x55" },
			{ TunkioFillType::CharacterFill, "\xAA" },
			{ TunkioFillType::SentenceFill, "\x92\x49\x24" },
			{ TunkioFillType::SentenceFill, "\x49\x24\x92" },
			{ TunkioFillType::SentenceFill, "\x24\x92\x49" },
			{ TunkioFillType::ZeroFill, {} },
			{ TunkioFillType::CharacterFill, "\x11" },
			{ TunkioFillType::CharacterFill, "\x22" },
			{ TunkioFillType::CharacterFill, "\x33" },
			{ TunkioFillType::CharacterFill, "\x44" },
			{ TunkioFillType::CharacterFill, "\x55" },
			{ TunkioFillType::CharacterFill, "\x66" },
			{ TunkioFillType::CharacterFill, "\x77" },
			{ TunkioFillType::CharacterFill, "\x88" },
			{ TunkioFillType::CharacterFill, "\x99" },
			{ TunkioFillType::CharacterFill, "\xAA" },
			{ TunkioFillType::CharacterFill, "\xBB" },
			{ TunkioFillType::CharacterFill, "\xCC" },
			{ TunkioFillType::CharacterFill, "\xDD" },
			{ TunkioFillType::CharacterFill, "\xEE" },
			{ TunkioFillType::OneFill, {} },
			{ TunkioFillType::SentenceFill, "\x92\x49\x24" },
			{ TunkioFillType::SentenceFill, "\x49\x24\x92" },
			{ TunkioFillType::SentenceFill, "\x24\x92\x49" },
			{ TunkioFillType::SentenceFill, "\x6D\xB6\xDB" },
			{ TunkioFillType::SentenceFill, "\xB6\xDB\x6D" },
			{ TunkioFillType::SentenceFill, "\xDB\x6D\xB6" },

			{ TunkioFillType::RandomFill, {} },
			{ TunkioFillType::RandomFill, {} },
			{ TunkioFillType::RandomFill, {} },
			{ TunkioFillType::RandomFill, {} },
		};

		do
		{
			m_model->clearPasses();

			std::shuffle(gutmannsRecipe.begin() + 4, gutmannsRecipe.end() - 4, engine);

			for (const auto& [type, value] : gutmannsRecipe)
			{
				m_model->addPass(type, false, value);
			}

		} while (QMessageBox::question(
			this,
			"Tunkio - Shuffle",
			"Gutmann's method requires random order of some of the passes.\n"
			"Click retry to shuffle, OK to accept.",
			QMessageBox::Retry | QMessageBox::Ok) == QMessageBox::Retry);
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
		if (m_tunkio.get() && m_tunkio->isRunning())
		{
			return;
		}

		ui->pushButtonRemovePass->setEnabled(!selected.isEmpty());
	});

	const auto enableStartButton = [this]()
	{
		ui->pushButtonClearPasses->setEnabled(!m_model->isEmpty());
		ui->pushButtonStart->setEnabled(!m_model->isEmpty() && m_tunkio.get());
	};

	connect(m_model, &QAbstractItemModel::rowsInserted, enableStartButton);
	connect(m_model, &QAbstractItemModel::rowsRemoved, enableStartButton);
	// connect(m_model, &QAbstractItemModel::modelReset, enableStartButton);
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
		const QString selected = dialog.selectedFiles().first();
		const QString file = QDir::toNativeSeparators(selected);

		ui->lineEditSelectedPath->setText(file);
		m_tunkio = std::make_shared<TunkioRunner>(file, TunkioTargetType::FileWipe);
		ui->pushButtonStart->setEnabled(!m_model->isEmpty() && m_tunkio.get());
	}
}

void MainWindow::onOpenDirectoryDialog()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::FileMode::Directory);
	dialog.setOption(QFileDialog::ShowDirsOnly, true);

	if (dialog.exec() == QFileDialog::Accepted)
	{
		const QString selected = dialog.selectedFiles().first();
		const QString directory = QDir::toNativeSeparators(selected);

		ui->lineEditSelectedPath->setText(directory);
		m_tunkio = std::make_shared<TunkioRunner>(directory, TunkioTargetType::DirectoryWipe);
		ui->pushButtonStart->setEnabled(!m_model->isEmpty() && m_tunkio.get());
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
		ui->pushButtonStart->setEnabled(!m_model->isEmpty() && m_tunkio.get());
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
					const QString selected = dialog.selectedFiles().first();
					const QString file = QDir::toNativeSeparators(selected);
					ui->lineEditFillValue->setText(file);
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
			ui->lineEditFillValue->setText("0xFF");
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
	QByteArray fill = ui->lineEditFillValue->text().toUtf8();

	switch (index)
	{
		case 0:
			return m_model->addPass(TunkioFillType::ZeroFill, verify);
		case 1:
			return m_model->addPass(TunkioFillType::OneFill, verify);
		case 2:
			return m_model->addPass(TunkioFillType::CharacterFill, verify, fill);
		case 3:
			return m_model->addPass(TunkioFillType::SentenceFill, verify, fill);
		case 4:
			return m_model->addPass(TunkioFillType::FileFill, verify, fill);
		case 5:
			return m_model->addPass(TunkioFillType::RandomFill, verify);
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
			message << QString("Fill value: ") << pass.fillValue.toHex(' ');

			QMessageBox::critical(this, "Tunkio - An error occurred", message.join('\n'));
			return;
		}
	}

	ui->pushButtonStart->setEnabled(false);
	ui->pushButtonCancel->setEnabled(true);
	ui->groupBoxAddPass->setEnabled(false);

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
	ui->groupBoxAddPass->setEnabled(true);
}

void MainWindow::onWipeCompleted(uint16_t, uint64_t)
{
	ui->pushButtonStart->setEnabled(true);
	ui->pushButtonCancel->setEnabled(false);
	ui->groupBoxAddPass->setEnabled(true);
}
