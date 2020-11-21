#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui
{
	class DriveSelectDialog;
}
QT_END_NAMESPACE

class DriveSelectDialog : public QDialog
{
	Q_OBJECT

public:
	DriveSelectDialog(QWidget* parent = nullptr);
	~DriveSelectDialog();

	QString selectedDrive();

private:
	Ui::DriveSelectDialog* ui;
};
