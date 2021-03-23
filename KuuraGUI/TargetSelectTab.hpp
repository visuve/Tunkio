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

signals:
	void targetTypeSelected(KuuraTargetType);

private:
	Ui::TargetSelectTab *ui;
	KuuraTargetType m_selectedTargetType = KuuraTargetType::FileWipe;
};

