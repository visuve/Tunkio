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
	void onTargetTypeSelected(KuuraTargetType);
	void onTargetPathsSelected(const QStringList&);
	void onTargetDrivesSelected(const QStringList&);
	void onAlgorithmSelected(const QVector<QPair<KuuraFillType, QByteArray>>&);
	void onOverWriteFinished();

	void dragEnterEvent(QDragEnterEvent* e);
	void dropEvent(QDropEvent* e);

private:
	Ui::MainWindow* ui = nullptr;
	TargetSelectTab* m_targetSelectTab = nullptr;
	PathSelectTab* m_pathSelectTab = nullptr;
	DriveSelectTab* m_driveSelectTab = nullptr;
	AlgorithmTab* m_algorithmTab = nullptr;
	ProgressTab* m_progressTab = nullptr;
	ResultsTab* m_resultsTab = nullptr;
};
