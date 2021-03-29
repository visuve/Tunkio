#pragma once

#include <QWidget>
#include <QFileDialog>

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

	void addPaths(QVector<QFileInfo>&& paths);
	const QVector<QFileInfo>& selectedPaths() const;

signals:
	void backRequested();
	void nextRequested();

private:
	void onAddPaths(QFileDialog::FileMode mode);

	Ui::PathSelectTab* ui;
};

