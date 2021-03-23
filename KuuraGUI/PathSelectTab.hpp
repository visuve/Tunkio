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

private:
	void onAddPaths(bool isDirectory);

	Ui::PathSelectTab* ui;
};

