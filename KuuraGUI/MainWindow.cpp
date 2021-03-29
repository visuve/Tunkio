#include "KuuraGUI-PCH.hpp"
#include "MainWindow.hpp"
#include "ui_MainWindow.h"

#include "TargetSelectTab.hpp"
#include "PathSelectTab.hpp"
#include "DriveSelectTab.hpp"
#include "AlgorithmTab.hpp"
#include "ProgressTab.hpp"
#include "ResultsTab.hpp"

MainWindow::MainWindow(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow()),
	m_targetSelectTab(new TargetSelectTab(ui->tabWidget)),
	m_pathSelectTab(new PathSelectTab(ui->tabWidget)),
	m_driveSelectTab(new DriveSelectTab(ui->tabWidget)),
	m_algorithmTab(new AlgorithmTab(ui->tabWidget)),
	m_progressTab(new ProgressTab(ui->tabWidget)),
	m_resultsTab(new ResultsTab(ui->tabWidget))
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

	connect(m_targetSelectTab, &TargetSelectTab::backRequested, this, &MainWindow::onBackRequested);
	connect(m_targetSelectTab, &TargetSelectTab::targetTypeSelected, this, &MainWindow::onTargetTypeSelected);
	connect(m_targetSelectTab, &TargetSelectTab::nextRequested, this, &MainWindow::onNextRequested);

	connect(m_pathSelectTab, &PathSelectTab::backRequested, this, &MainWindow::onBackRequested);
	connect(m_pathSelectTab, &PathSelectTab::nextRequested, this, &MainWindow::onNextRequested);

	connect(m_driveSelectTab, &DriveSelectTab::backRequested, this, &MainWindow::onBackRequested);
	connect(m_driveSelectTab, &DriveSelectTab::nextRequested, this, &MainWindow::onNextRequested);

	connect(m_algorithmTab, &AlgorithmTab::backRequested, this, &MainWindow::onBackRequested);
	connect(m_algorithmTab, &AlgorithmTab::nextRequested, this, &MainWindow::onNextRequested);

	connect(m_progressTab, &ProgressTab::nextRequested, this, &MainWindow::onNextRequested);

	ui->tabWidget->insertTab(1, m_targetSelectTab, "Target");
	ui->tabWidget->insertTab(2, m_pathSelectTab, "Path");
	ui->tabWidget->insertTab(3, m_driveSelectTab, "Path");
	ui->tabWidget->insertTab(4, m_algorithmTab, "Algorithm");
	ui->tabWidget->insertTab(5, m_progressTab, "Progress");
	ui->tabWidget->insertTab(6, m_resultsTab, "Results");
	ui->tabWidget->setTabVisible(3, false);

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
			switch (m_targetSelectTab->selectedTargetType())
			{
				case KuuraTargetType::FileWipe:
				case KuuraTargetType::DirectoryWipe:
					ui->tabWidget->setCurrentIndex(2);
					return;
				case KuuraTargetType::DriveWipe:
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
		case 0:
		{
			ui->tabWidget->setCurrentIndex(1);
			return;
		}
		case 1:
		{
			switch (m_targetSelectTab->selectedTargetType())
			{
				case KuuraTargetType::FileWipe:
				case KuuraTargetType::DirectoryWipe:
					ui->tabWidget->setCurrentIndex(2);
					return;
				case KuuraTargetType::DriveWipe:
					ui->tabWidget->setCurrentIndex(3);
					return;
			}
			return;
		}
		case 2:
		case 3:
		{
			ui->tabWidget->setCurrentIndex(4);
			return;
		}
		case 4:
		{
			ui->tabWidget->setCurrentIndex(5);
			return;
		}
		case 5:
		{
			ui->tabWidget->setCurrentIndex(6);
			return;
		}
	}
}

void MainWindow::onTargetTypeSelected(KuuraTargetType tt)
{
	switch (tt)
	{
		case FileWipe:
		case DirectoryWipe:
			ui->tabWidget->setTabVisible(2, true);
			ui->tabWidget->setTabVisible(3, false);
			return;
		case DriveWipe:
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
	QStringList paths;

	for (const QUrl& url : e->mimeData()->urls())
	{
		paths << QDir::toNativeSeparators(url.toLocalFile());
	}

	m_pathSelectTab->addPaths(paths);
}
