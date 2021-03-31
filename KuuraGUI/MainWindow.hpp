#pragma once

#include "WipePassModel.hpp"
#include "KuuraRunner.hpp"
#include "TextEditorDialog.hpp"

#include <QMainWindow>

namespace Ui
{
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow();

	bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
	void initMenu();
	void onOpenFileDialog();
	void onOpenDirectoryDialog();
	void onOpenDriveDialog();
	void onFillTypeSelectionChanged(int index);
	void addPass();
	void removePass();
	void clearPasses();
	void onAbout();
	void startWipe();
	void onError(const QString& path, KuuraStage stage, uint16_t pass, uint64_t bytesWritten, uint32_t errorCode);
	void onWipeCompleted(uint16_t pass, uint64_t totalBytesWritten);

private:
	Ui::MainWindow* ui = nullptr;
	WipePassModel* m_model = nullptr;
	TextEditorDialog* m_textEditDialog = nullptr;
	std::shared_ptr<KuuraRunner> m_kuura;
};
