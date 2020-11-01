#include "MainWindow.hpp"
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

MainWindow::MainWindow(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow())
{
	ui->setupUi(this);

	ui->actionOpen->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon));
	connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::OnOpenDirectoryDialog);

	ui->actionExit->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton));
	connect(ui->actionExit, &QAction::triggered, qApp, &QApplication::quit);

	ui->actionAbout->setIcon(QApplication::style()->standardIcon(QStyle::SP_MessageBoxInformation));
	connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::OnAbout);

	ui->actionLicenses->setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarMenuButton));
	connect(ui->actionLicenses, &QAction::triggered, [this]()
	{
		QMessageBox::aboutQt(this, "Tunkio");
	});
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::OnOpenDirectoryDialog()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::AnyFile);

	if (dialog.exec() == QFileDialog::Accepted)
	{
		const QString path = dialog.selectedFiles().first();
		ui->lineEditSelectedPath->setText(QDir::toNativeSeparators(path));
	}
}

void MainWindow::OnAbout()
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
