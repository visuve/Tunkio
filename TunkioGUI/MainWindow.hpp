#pragma once

#include <QMainWindow>
#include "TunkioInstance.hpp"
#include "WipePassModel.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

private slots:
	void onOpenFileDialog();
	void onOpenDirectoryDialog();
	void onOpenDriveDialog();
	void onAbout();

private:
	void attachCallbacks();
	Ui::MainWindow* ui;
	WipePassModel* m_model;
	Tunkio::Instance m_tunkio;
};
