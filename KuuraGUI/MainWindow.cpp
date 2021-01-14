#include "KuuraGUI-PCH.hpp"
#include "MainWindow.hpp"
#include "ProgressBarDelegate.hpp"
#include "DriveSelectDialog.hpp"
#include "ui_MainWindow.h"

#include "../KuuraLIB/KuuraErrorCodes.hpp"

QString toString(KuuraStage type)
{
	switch (type)
	{
		case KuuraStage::Open:
			return "opening";
		case KuuraStage::Unmount:
			return "unmounting";
		case KuuraStage::Size:
			return "querying size";
		case KuuraStage::Write:
			return "writing";
		case KuuraStage::Verify:
			return "verifying";
		case KuuraStage::Delete:
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
		QMessageBox::aboutQt(this, "Kuura");
	});

	connect(ui->actionNIST_SP_800_88_Rev_1, &QAction::triggered, [this]()
	{
		ui->groupBoxAddPass->setEnabled(false);
		m_model->clearPasses();
		m_model->addPass(KuuraFillType::ZeroFill, false);
	});

	connect(ui->actionBritish_HMG_Infosec_Standard_5_Baseline_Standard, &QAction::triggered, [this]()
	{
		ui->groupBoxAddPass->setEnabled(false);
		m_model->clearPasses();
		m_model->addPass(KuuraFillType::RandomFill, true);
	});

	connect(ui->actionBritish_HMG_Infosec_Standard_5_Enhanced_Standard, &QAction::triggered, [this]()
	{
		ui->groupBoxAddPass->setEnabled(false);
		m_model->clearPasses();
		m_model->addPass(KuuraFillType::OneFill, false);
		m_model->addPass(KuuraFillType::ZeroFill, false);
		m_model->addPass(KuuraFillType::RandomFill, true);
	});

	connect(ui->actionCommunications_Security_Establishment_Canada_ITSG_06, &QAction::triggered, [this]()
	{
		ui->groupBoxAddPass->setEnabled(false);
		m_model->clearPasses();
		m_model->addPass(KuuraFillType::ZeroFill, false);
		m_model->addPass(KuuraFillType::OneFill, false);
		m_model->addPass(KuuraFillType::RandomFill, false);
	});

	connect(ui->actionGerman_Federal_Office_for_Information_Security, &QAction::triggered, [this]()
	{
		QMessageBox::critical(
			this,
			"Kuura - Not implemented yet!",
			"This feature is not yet implemented");
	});

	connect(ui->actionU_S_DoD_Unclassified_Computer_Hard_Drive_Disposition, &QAction::triggered, [this]()
	{
		ui->groupBoxAddPass->setEnabled(false);
		m_model->clearPasses();
		m_model->addPass(KuuraFillType::ByteFill, false, "\x55");
		m_model->addPass(KuuraFillType::ByteFill, false, "\xAA");
		m_model->addPass(KuuraFillType::RandomFill,  false);
	});

	connect(ui->actionBruce_Schneier_s_Algorithm, &QAction::triggered, [this]()
	{
		ui->groupBoxAddPass->setEnabled(false);
		m_model->clearPasses();
		m_model->addPass(KuuraFillType::OneFill, false);
		m_model->addPass(KuuraFillType::ZeroFill, false);
		m_model->addPass(KuuraFillType::RandomFill, false);
		m_model->addPass(KuuraFillType::RandomFill, false);
		m_model->addPass(KuuraFillType::RandomFill, false);
		m_model->addPass(KuuraFillType::RandomFill, false);
		m_model->addPass(KuuraFillType::RandomFill, false);
	});

	connect(ui->actionPeter_Gutmann_s_Algorithm, &QAction::triggered, [this]()
	{
		ui->groupBoxAddPass->setEnabled(false);
		thread_local std::random_device device;
		thread_local std::mt19937_64 engine(device());

		std::array<std::pair<KuuraFillType, QByteArray>, 35> gutmannsRecipe =
		{
			std::make_pair(KuuraFillType::RandomFill, QByteArray()),
			{ KuuraFillType::RandomFill, {} },
			{ KuuraFillType::RandomFill, {} },
			{ KuuraFillType::RandomFill, {} },

			{ KuuraFillType::ByteFill, "\x55" },
			{ KuuraFillType::ByteFill, "\xAA" },
			{ KuuraFillType::SequenceFill, "\x92\x49\x24" },
			{ KuuraFillType::SequenceFill, "\x49\x24\x92" },
			{ KuuraFillType::SequenceFill, "\x24\x92\x49" },
			{ KuuraFillType::ZeroFill, {} },
			{ KuuraFillType::ByteFill, "\x11" },
			{ KuuraFillType::ByteFill, "\x22" },
			{ KuuraFillType::ByteFill, "\x33" },
			{ KuuraFillType::ByteFill, "\x44" },
			{ KuuraFillType::ByteFill, "\x55" },
			{ KuuraFillType::ByteFill, "\x66" },
			{ KuuraFillType::ByteFill, "\x77" },
			{ KuuraFillType::ByteFill, "\x88" },
			{ KuuraFillType::ByteFill, "\x99" },
			{ KuuraFillType::ByteFill, "\xAA" },
			{ KuuraFillType::ByteFill, "\xBB" },
			{ KuuraFillType::ByteFill, "\xCC" },
			{ KuuraFillType::ByteFill, "\xDD" },
			{ KuuraFillType::ByteFill, "\xEE" },
			{ KuuraFillType::OneFill, {} },
			{ KuuraFillType::SequenceFill, "\x92\x49\x24" },
			{ KuuraFillType::SequenceFill, "\x49\x24\x92" },
			{ KuuraFillType::SequenceFill, "\x24\x92\x49" },
			{ KuuraFillType::SequenceFill, "\x6D\xB6\xDB" },
			{ KuuraFillType::SequenceFill, "\xB6\xDB\x6D" },
			{ KuuraFillType::SequenceFill, "\xDB\x6D\xB6" },

			{ KuuraFillType::RandomFill, {} },
			{ KuuraFillType::RandomFill, {} },
			{ KuuraFillType::RandomFill, {} },
			{ KuuraFillType::RandomFill, {} },
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
			"Kuura - Shuffle",
			"Gutmann's method requires random order of some of the passes.\n"
			"Click retry to shuffle, OK to accept.",
			QMessageBox::Retry | QMessageBox::Ok) == QMessageBox::Retry);
	});

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
		if (m_kuura.get() && m_kuura->isRunning())
		{
			return;
		}

		ui->pushButtonRemovePass->setEnabled(!selected.isEmpty());
	});

	const auto enableStartButton = [this]()
	{
		ui->pushButtonClearPasses->setEnabled(!m_model->isEmpty());
		ui->pushButtonStart->setEnabled(!m_model->isEmpty() && m_kuura.get());
	};

	connect(m_model, &QAbstractItemModel::rowsInserted, enableStartButton);
	connect(m_model, &QAbstractItemModel::rowsRemoved, enableStartButton);
	// connect(m_model, &QAbstractItemModel::modelReset, enableStartButton);
	// TODO: attach enableStartButton when m_kuura is initialized

	m_textEditDialog = new TextEditorDialog(ui->lineEditFillValue);

	auto indexChanged = static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged);

	connect(ui->comboBoxFillType, indexChanged, [this](int index)
	{
		this->onFillTypeSelectionChanged(index);
	});

	connect(ui->lineEditFillValue, &QLineEdit::textEdited, [&](const QString& text)
	{
		if (ui->comboBoxFillType->currentIndex() == 3)
		{
			m_textEditDialog->setText(text);
		}
	});

	connect(m_textEditDialog, &TextEditorDialog::accepted, [this]()
	{
		QString sequence = m_textEditDialog->text();
		ui->lineEditFillValue->setText(sequence);
	});

	ui->lineEditFillValue->installEventFilter(this);
}

MainWindow::~MainWindow()
{
	m_kuura.reset();
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
		m_kuura = std::make_shared<KuuraRunner>(
			file,
			KuuraTargetType::FileWipe,
			ui->checkBoxDelete->isChecked());
		ui->pushButtonStart->setEnabled(!m_model->isEmpty() && m_kuura.get());
		ui->groupBoxPathSelect->setEnabled(false);
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

		m_kuura = std::make_shared<KuuraRunner>(
			directory,
			KuuraTargetType::DirectoryWipe,
			ui->checkBoxDelete->isChecked());

		ui->pushButtonStart->setEnabled(!m_model->isEmpty() && m_kuura.get());
		ui->groupBoxPathSelect->setEnabled(false);
	}
}

void MainWindow::onOpenDriveDialog()
{
	DriveSelectDialog dialog(this);

	ui->checkBoxDelete->setCheckState(Qt::CheckState::Unchecked);

	if (dialog.exec() == QDialog::Accepted)
	{
		const QString drive = dialog.selectedDrive();
		ui->lineEditSelectedPath->setText(drive);

		m_kuura = std::make_shared<KuuraRunner>(
			drive,
			KuuraTargetType::DriveWipe,
			false);

		ui->pushButtonStart->setEnabled(!m_model->isEmpty() && m_kuura.get());
		ui->groupBoxPathSelect->setEnabled(false);
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
			return m_model->addPass(KuuraFillType::ZeroFill, verify);
		case 1:
			return m_model->addPass(KuuraFillType::OneFill, verify);
		case 2:
			return m_model->addPass(KuuraFillType::ByteFill, verify, fill);
		case 3:
			return m_model->addPass(KuuraFillType::SequenceFill, verify, fill);
		case 4:
			return m_model->addPass(KuuraFillType::FileFill, verify, fill);
		case 5:
			return m_model->addPass(KuuraFillType::RandomFill, verify);
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
	text << "Kuura - Data Erasure Tool version 0.1.";
	text << "";
	text << "Kuura is yet another data erasure tool.";
	text << "";
	text << "With Kuura, you can add passes to wipe the data according to your will,"
		"hence fullfilling even the most obscure standards";
	text << "";
	text << "Kuura is licenced under GPLv2.";
	text << "";
	text << "This GUI uses Qt version " + QString(qVersion()).append('.');
	text << "See 'About Qt' for more details.";
	text << "";

	QMessageBox::about(this, "Kuura", text.join('\n'));
}

void MainWindow::startWipe()
{
	Q_ASSERT(m_kuura.get());
	Q_ASSERT(!m_kuura->isRunning());
	Q_ASSERT(!m_model->isEmpty());

	m_model->clearStats();

	for (const WipePassModel::Pass& pass : m_model->passes())
	{
		if (!m_kuura->addPass(pass.fillType, pass.fillValue, pass.verify))
		{
			QStringList message = { "Failed to add pass!" };
			message << QString("Pass type: %1").arg(Ui::fillTypeToString(pass.fillType));
			message << QString("Fill value: ") << pass.fillValue.toHex(' ');

			QMessageBox::critical(this, "Kuura - An error occurred", message.join('\n'));
			return;
		}
	}

	ui->pushButtonStart->setEnabled(false);
	ui->pushButtonCancel->setEnabled(true);
	ui->groupBoxAddPass->setEnabled(false);

	connect(m_kuura.get(), &KuuraRunner::wipeStarted, m_model, &WipePassModel::onWipeStarted);
	connect(m_kuura.get(), &KuuraRunner::passStarted, m_model, &WipePassModel::onPassStarted);
	connect(m_kuura.get(), &KuuraRunner::passProgressed, m_model, &WipePassModel::onPassProgressed);
	connect(m_kuura.get(), &KuuraRunner::passFinished, m_model, &WipePassModel::onPassFinished);
	connect(m_kuura.get(), &KuuraRunner::wipeCompleted, m_model, &WipePassModel::onWipeCompleted);
	connect(m_kuura.get(), &KuuraRunner::wipeCompleted, this, &MainWindow::onWipeCompleted);
	connect(m_kuura.get(), &KuuraRunner::errorOccurred, this, &MainWindow::onError);

	connect(ui->pushButtonCancel, &QPushButton::clicked, m_kuura.get(), &KuuraRunner::stop);

	m_kuura->start();
}

void MainWindow::onError(KuuraStage stage, uint16_t pass, uint64_t bytesWritten, uint32_t errorCode)
{
	QStringList message = { QString("An error occurred while %1!\n").arg(toString(stage)) };
	message << QString("Pass: %1").arg(pass);
	message << QString("Bytes written: %1").arg(bytesWritten);
	message << QString("Operating system error code: %1").arg(errorCode);
	message << "Detailed description: " <<
		QString::fromStdString(Kuura::SystemErrorCodeToString(errorCode));

	QMessageBox::critical(this, "Kuura - An error occurred", message.join('\n'));

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
