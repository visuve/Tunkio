#pragma once

#include <QWidget>

namespace Ui
{
	class DriveSelectTab;
}

class DriveSelectTab : public QWidget
{
	Q_OBJECT

public:
	explicit DriveSelectTab(QWidget *parent = nullptr);
	~DriveSelectTab();

	void refreshDrives();

	QStringList selectedDrives() const;

signals:
	void backRequested();
	void nextRequested();

private:
	Ui::DriveSelectTab *ui;
};
