#pragma once

#include <QWidget>

#include "../KuuraAPI/KuuraAPI.h"

namespace Ui
{
	class PathSelectTab;
}

class PathSelectTab : public QWidget
{
	Q_OBJECT

public:
	explicit PathSelectTab(QWidget *parent = nullptr);
	~PathSelectTab();

signals:
	void backRequested();
	void targetPathsSelected(const QStringList&);

private:
	void onAddPaths(QFileDialog::FileMode mode);

	Ui::PathSelectTab* ui;
};

