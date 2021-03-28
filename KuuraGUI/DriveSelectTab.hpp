#pragma once

#include <QWidget>

namespace Ui {
	class DriveSelectTab;
}

class DriveSelectTab : public QWidget
{
	Q_OBJECT

public:
	explicit DriveSelectTab(QWidget *parent = nullptr);
	~DriveSelectTab();

signals:
	void backRequested();
	void targetDrivesSelected(const QStringList&);

private:
	Ui::DriveSelectTab *ui;
};

