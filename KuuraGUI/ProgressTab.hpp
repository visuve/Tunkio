#pragma once

#include <QWidget>

namespace Ui
{
	class ProgressTab;
}

class ProgressTab : public QWidget
{
	Q_OBJECT

public:
	explicit ProgressTab(QWidget *parent = nullptr);
	~ProgressTab();

private:
	Ui::ProgressTab *ui;
};
