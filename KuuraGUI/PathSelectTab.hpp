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
	const QVector<QPair<QFileInfo, bool>>& selectedPaths() const;

signals:
	void backRequested();
	void nextRequested();

private:
	void onAddPaths(QFileDialog::FileMode mode);
	void onRemoveSelected();

	Ui::PathSelectTab* ui;
};

