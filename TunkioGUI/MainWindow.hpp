#pragma once

#include "WipePassModel.hpp"
#include "TunkioRunner.hpp"

namespace Ui
{
	class MainWindow;
}

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
	void onFillTypeSelectionChanged(int index);
	void addPass();
	void onAbout();
	void onError(TunkioStage stage, uint16_t pass, uint64_t bytesWritten, uint32_t errorCode);
	void startWipe();

private:
	Ui::MainWindow* ui;
	WipePassModel* m_model;
	std::shared_ptr<TunkioRunner> m_tunkio;
};
