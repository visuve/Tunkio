#include "KuuraGUI-PCH.hpp"
#include "MainWindow.hpp"
#include "ui_MainWindow.h"

#include "KuuraRunner.hpp"
#include "TargetSelectTab.hpp"
#include "PathSelectTab.hpp"
#include "DriveSelectTab.hpp"
#include "AlgorithmTab.hpp"
#include "ProgressTab.hpp"
#include "ResultsTab.hpp"
#include "../KuuraResources/KuuraVersion.h"

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
	ui(new Ui::MainWindow()),
	_targetSelectTab(new TargetSelectTab(ui->tabWidget)),
	_pathSelectTab(new PathSelectTab(ui->tabWidget)),
	_driveSelectTab(new DriveSelectTab(ui->tabWidget)),
	_algorithmTab(new AlgorithmTab(ui->tabWidget)),
	_progressTab(new ProgressTab(ui->tabWidget)),
	_resultsTab(new ResultsTab(ui->tabWidget))
{
	ui->setupUi(this);

	ui->actionExit->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton));
	connect(ui->actionExit, &QAction::triggered, qApp, &QApplication::quit);

	ui->actionAbout->setIcon(QApplication::style()->standardIcon(QStyle::SP_MessageBoxInformation));
	connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAbout);

	ui->actionLicences->setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarMenuButton));
	connect(ui->actionLicences, &QAction::triggered, [this]()
	{
		QMessageBox::aboutQt(this, "Kuura");
	});

	connect(ui->pushButtonNext, &QPushButton::clicked, this, &MainWindow::onNextRequested);

	connect(_targetSelectTab, &TargetSelectTab::backRequested, this, &MainWindow::onBackRequested);
	connect(_targetSelectTab, &TargetSelectTab::targetTypeSelected, this, &MainWindow::onTargetTypeSelected);
	connect(_targetSelectTab, &TargetSelectTab::nextRequested, this, &MainWindow::onNextRequested);

	connect(_pathSelectTab, &PathSelectTab::backRequested, this, &MainWindow::onBackRequested);
	connect(_pathSelectTab, &PathSelectTab::nextRequested, this, &MainWindow::onNextRequested);

	connect(_driveSelectTab, &DriveSelectTab::backRequested, this, &MainWindow::onBackRequested);
	connect(_driveSelectTab, &DriveSelectTab::nextRequested, this, &MainWindow::onNextRequested);

	connect(_algorithmTab, &AlgorithmTab::backRequested, this, &MainWindow::onBackRequested);
	connect(_algorithmTab, &AlgorithmTab::nextRequested, this, &MainWindow::onNextRequested);

	connect(_progressTab, &ProgressTab::nextRequested, this, &MainWindow::onNextRequested);

	ui->tabWidget->insertTab(1, _targetSelectTab, "Target");
	ui->tabWidget->insertTab(2, _pathSelectTab, "Path");
	ui->tabWidget->insertTab(3, _driveSelectTab, "Path");
	ui->tabWidget->insertTab(4, _algorithmTab, "Algorithm");
	ui->tabWidget->insertTab(5, _progressTab, "Progress");
	ui->tabWidget->insertTab(6, _resultsTab, "Results");
	ui->tabWidget->setTabVisible(3, false);

	connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);

	setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
	delete ui;
	qDebug();
}

void MainWindow::onAbout()
{
	QStringList text;
	text << "Kuura - Data Erasure Tool version " << KuuraVersionString;
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

void MainWindow::onBackRequested()
{
	switch (ui->tabWidget->currentIndex())
	{
		case 1:
		{
			ui->tabWidget->setCurrentIndex(0);
			return;
		}
		case 2:
		case 3:
		{
			ui->tabWidget->setCurrentIndex(1);
			return;
		}
		case 4:
		{
			switch (_targetSelectTab->selectedTargetType())
			{
				case KuuraTargetType::FileOverwrite:
				case KuuraTargetType::DirectoryOverwrite:
					ui->tabWidget->setCurrentIndex(2);
					return;
				case KuuraTargetType::DriveOverwrite:
					ui->tabWidget->setCurrentIndex(3);
					return;
			}
			return;
		}
	}
}

void MainWindow::onNextRequested()
{
	switch (ui->tabWidget->currentIndex())
	{
		case 0: // Welcome
		{
			ui->tabWidget->setCurrentIndex(1);
			return;
		}
		case 1: // Target
		{
			switch (_targetSelectTab->selectedTargetType())
			{
				case KuuraTargetType::FileOverwrite:
				case KuuraTargetType::DirectoryOverwrite:
					ui->tabWidget->setCurrentIndex(2);
					return;
				case KuuraTargetType::DriveOverwrite:
					ui->tabWidget->setCurrentIndex(3);
					return;
			}
			return;
		}
		case 2: // Path
		case 3: // Drive
		{
			ui->tabWidget->setCurrentIndex(4);
			return;
		}
		case 4: // Algorithm
		{
			ui->tabWidget->setCurrentIndex(5);
			return;
		}
		case 5: // Progress
		{
			ui->tabWidget->setCurrentIndex(6);
			return;
		}
	}
}

void MainWindow::onTabChanged(int index)
{
	qDebug() << "Changed to:" << index;

	switch (index)
	{
		case 3: // Drive
			_driveSelectTab->refreshDrives();
			return;
		case 5:
			runKuura();
			return;
	}
}

void MainWindow::onTargetTypeSelected(KuuraTargetType tt)
{
	switch (tt)
	{
		case FileOverwrite:
		case DirectoryOverwrite:
			ui->tabWidget->setTabVisible(2, true);
			ui->tabWidget->setTabVisible(3, false);
			return;
		case DriveOverwrite:
			ui->tabWidget->setTabVisible(2, false);
			ui->tabWidget->setTabVisible(3, true);
			return;
	}
}

void MainWindow::dragEnterEvent(QDragEnterEvent* e)
{
	if (e->mimeData()->hasUrls() && ui->tabWidget->currentIndex() == 2)
	{
		e->acceptProposedAction();
	}
}
void MainWindow::dropEvent(QDropEvent* e)
{
	QVector<QFileInfo> paths;

	for (const QUrl& url : e->mimeData()->urls())
	{
		paths.emplace_back(QDir::toNativeSeparators(url.toLocalFile()));
	}

	_pathSelectTab->addPaths(std::move(paths));
}


void MainWindow::runKuura()
{
	auto kuuraRunner = new KuuraRunner(this);

	for (const QPair<QFileInfo, bool>& selectedPath : _pathSelectTab->selectedPaths())
	{
		if (selectedPath.first.isFile())
		{
			if (kuuraRunner->addTarget(
				KuuraTargetType::FileOverwrite,
				selectedPath.first,
				selectedPath.second))
			{
				_progressTab->addTarget(selectedPath.first);
				continue;
			}
		}

		if (selectedPath.first.isDir())
		{
			if (kuuraRunner->addTarget(
				KuuraTargetType::DirectoryOverwrite,
				selectedPath.first,
				selectedPath.second))
			{
				_progressTab->addTarget(selectedPath.first);
				continue;
			}
		}

		qCritical() << "Should never reach here!";
	}

	for (const QString& selectedDrive : _driveSelectTab->selectedDrives())
	{
		if (kuuraRunner->addTarget(KuuraTargetType::DriveOverwrite, selectedDrive, false))
		{
			_progressTab->addTarget(selectedDrive);
			continue;
		}

		qCritical() << "Should never reach here!";
	}

	for (const AlgorithmTab::OverwritePass& pass : _algorithmTab->selectedPasses())
	{
		if (kuuraRunner->addPass(pass.fillType, pass.fillValue, pass.verify))
		{
			_progressTab->addPass({ pass.fillType, pass.verify });
			continue;
		}

		qCritical() << "Should never reach here!";
	}

	connect(kuuraRunner, &KuuraRunner::overwriteStarted, _progressTab, &ProgressTab::onOverwriteStarted);
	connect(kuuraRunner, &KuuraRunner::targetStarted, _progressTab, &ProgressTab::onTargetStarted);
	connect(kuuraRunner, &KuuraRunner::passStarted, _progressTab, &ProgressTab::onPassStarted);
	connect(kuuraRunner, &KuuraRunner::passProgressed, _progressTab, &ProgressTab::onPassProgressed);
	connect(kuuraRunner, &KuuraRunner::passCompleted, _progressTab, &ProgressTab::onPassCompleted);
	connect(kuuraRunner, &KuuraRunner::targetCompleted, _progressTab, &ProgressTab::onTargetCompleted);
	connect(kuuraRunner, &KuuraRunner::overwriteCompleted, _progressTab, &ProgressTab::onOverwriteCompleted);

	connect(kuuraRunner, &KuuraRunner::overwriteCompleted, this, []()
	{
		qDebug() << "Completed";
	});

	connect(kuuraRunner, &QThread::finished, kuuraRunner, &QObject::deleteLater); // TODO: causes crash

	kuuraRunner->start();
}


void MainWindow::onError(const std::filesystem::path& path, KuuraStage stage, uint16_t pass, uint64_t bytesWritten, uint32_t errorCode)
{
	qWarning() << path << stage << pass << bytesWritten << errorCode;

	_progressTab->onError();

	QStringList message =
	{
		QString("An error occurred while %1 %2!\n")
		.arg(toString(stage))
		.arg(QString::fromStdString(path.string()))
	};

	message << QString("Pass: %1").arg(pass);
	message << QString("Bytes written: %1").arg(bytesWritten);
	message << QString("Operating system error code: %1").arg(errorCode);

	/*message << "Detailed description: " <<
		QString::fromStdString(Kuura::SystemErrorCodeToString(errorCode));*/

	QMessageBox::critical(this, "Kuura - An error occurred", message.join('\n'));
}
