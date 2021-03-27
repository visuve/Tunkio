#pragma once

#include <QWidget>

namespace Ui {
	class AlgorithmTab;
}

class AlgorithmTab : public QWidget
{
	Q_OBJECT

public:
	explicit AlgorithmTab(QWidget *parent = nullptr);
	~AlgorithmTab();

signals:
	void algorithmSelected(const QVector<QPair<KuuraFillType, QByteArray>>&);

private:
	void presetChanged(int index);

	Ui::AlgorithmTab *ui;
};

