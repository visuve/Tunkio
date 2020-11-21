#pragma once

#include <QMainWindow>
#include <memory>
#include "WipePassModel.hpp"
#include "TunkioRunner.hpp"

QT_BEGIN_NAMESPACE
namespace Ui
{
	class MainWindow;
}
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
	void addPass();
	void onAbout();
	void onError(TunkioStage stage, uint16_t pass, uint64_t bytesWritten, uint32_t errorCode);

private:
	void attachCallbacks();
	Ui::MainWindow* ui;
	WipePassModel* m_model;
	std::shared_ptr<TunkioRunner> m_tunkio;
};
