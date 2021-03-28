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

	QVector<QPair<KuuraFillType, QByteArray>> selectedAlgorithms();

signals:
	void backRequested();
	void nextRequested();

private:
	void presetChanged(int index);

	Ui::AlgorithmTab *ui;
};

