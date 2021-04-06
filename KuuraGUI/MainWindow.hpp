#pragma once

#include <QMainWindow>

class TargetSelectTab;
class PathSelectTab;
class DriveSelectTab;
class AlgorithmTab;
class ProgressTab;
class ResultsTab;

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

	void onAbout();
	void onBackRequested();
	void onNextRequested();
	void onTabChanged(int index);
	void onTargetTypeSelected(KuuraTargetType);

	void dragEnterEvent(QDragEnterEvent* e);
	void dropEvent(QDropEvent* e);

private:
	Ui::MainWindow* ui = nullptr;
	TargetSelectTab* _targetSelectTab = nullptr;
	PathSelectTab* _pathSelectTab = nullptr;
	DriveSelectTab* _driveSelectTab = nullptr;
	AlgorithmTab* _algorithmTab = nullptr;
	ProgressTab* _progressTab = nullptr;
	ResultsTab* _resultsTab = nullptr;
};
