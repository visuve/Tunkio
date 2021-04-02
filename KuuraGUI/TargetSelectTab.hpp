#pragma once

#include <QWidget>

#include "../KuuraAPI/KuuraAPI.h"

namespace Ui {
	class TargetSelectTab;
}

class TargetSelectTab : public QWidget
{
	Q_OBJECT

public:
	explicit TargetSelectTab(QWidget *parent = nullptr);
	~TargetSelectTab();

	KuuraTargetType selectedTargetType() const;

signals:
	void targetTypeSelected(KuuraTargetType);
	void nextRequested();
	void backRequested();

private:
	Ui::TargetSelectTab *ui;
	KuuraTargetType _selectedTargetType = KuuraTargetType::FileOverwrite;
};

