#include "MainWindow.hpp"
#include "WipePassModel.hpp"
#include "ProgressBarDelegate.hpp"
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

	ui->tableViewWipePasses->setModel(new WipePassModel(this));
	ui->tableViewWipePasses->setItemDelegateForColumn(7, new ProgressBarDelegate(this));
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
	}
}

void MainWindow::onOpenDriveDialog()
{

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
