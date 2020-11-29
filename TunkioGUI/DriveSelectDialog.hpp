#pragma once

namespace Ui
{
	class DriveSelectDialog;
}

class DriveSelectDialog : public QDialog
{
	Q_OBJECT

public:
	explicit DriveSelectDialog(QWidget* parent = nullptr);
	~DriveSelectDialog();

	QString selectedDrive();

private:
	Ui::DriveSelectDialog* ui;
};
