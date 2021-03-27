#include "KuuraGUI-PCH.hpp"
#include "MainWindow.hpp"
#include "ui_MainWindow.h"

#include "TargetSelectTab.hpp"
#include "PathSelectTab.hpp"
#include "DriveSelectTab.hpp"
#include "AlgorithmTab.hpp"
#include "ProgressTab.hpp"

MainWindow::MainWindow(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow()),
	m_targetSelectTab(new TargetSelectTab(ui->tabWidget)),
	m_pathSelectTab(new PathSelectTab(ui->tabWidget)),
	m_driveSelectTab(new DriveSelectTab(ui->tabWidget)),
	m_algorithmTab(new AlgorithmTab(ui->tabWidget)),
	m_progressTab(new ProgressTab(ui->tabWidget))
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

	connect(ui->pushButtonNext, &QPushButton::clicked, [this]()
	{
		ui->tabWidget->setCurrentIndex(1);
	});

	connect(m_targetSelectTab, &TargetSelectTab::targetTypeSelected, this, &MainWindow::onTargetTypeSelected);
	connect(m_pathSelectTab, &PathSelectTab::targetPathsSelected, this, &MainWindow::onTargetPathsSelected);
	connect(m_driveSelectTab, &DriveSelectTab::targetDrivesSelected, this, &MainWindow::onTargetDrivesSelected);
	connect(m_algorithmTab, &AlgorithmTab::algorithmSelected, this, &MainWindow::onAlgorithmSelected);

	ui->tabWidget->insertTab(1, m_targetSelectTab, "Target");
	ui->tabWidget->insertTab(2, m_pathSelectTab, "Path");
	ui->tabWidget->insertTab(3, m_driveSelectTab, "Path");
	ui->tabWidget->insertTab(4, m_algorithmTab, "Algorithm");
	ui->tabWidget->insertTab(5, m_progressTab, "Progress");
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

void MainWindow::onTargetTypeSelected(KuuraTargetType tt)
{
	switch (tt)
	{
		case FileWipe:
		case DirectoryWipe:
			ui->tabWidget->setTabVisible(0, true);
			ui->tabWidget->setTabVisible(1, true);
			ui->tabWidget->setTabVisible(2, true);
			ui->tabWidget->setTabVisible(3, false);
			ui->tabWidget->setCurrentWidget(m_pathSelectTab);
			return;
		case DriveWipe:
			ui->tabWidget->setTabVisible(0, true);
			ui->tabWidget->setTabVisible(1, true);
			ui->tabWidget->setTabVisible(2, false);
			ui->tabWidget->setTabVisible(3, true);
			ui->tabWidget->setCurrentWidget(m_driveSelectTab);
			return;
	}
}

void MainWindow::onTargetPathsSelected(const QStringList&)
{
	ui->tabWidget->setCurrentWidget(m_algorithmTab);
}

void MainWindow::onTargetDrivesSelected(const QStringList&)
{
	ui->tabWidget->setCurrentWidget(m_algorithmTab);
}

void MainWindow::onAlgorithmSelected(const QVector<QPair<KuuraFillType, QByteArray>>&)
{
	ui->tabWidget->setCurrentWidget(m_progressTab);
}

void MainWindow::dragEnterEvent(QDragEnterEvent* e)
{
	if (e->mimeData()->hasUrls() && ui->tabWidget->isTabVisible(2))
	{
		e->acceptProposedAction();
	}
}
void MainWindow::dropEvent(QDropEvent* e)
{
	for(const QUrl& url : e->mimeData()->urls())
	{
		QString fileName = url.toLocalFile();
		qDebug() << "Dropped file:" << fileName;
	}
}
