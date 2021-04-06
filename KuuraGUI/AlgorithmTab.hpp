#pragma once

#include <QWidget>
#include "../KuuraAPI/KuuraAPI.h"

namespace Ui
{
	class AlgorithmTab;
}

class AlgorithmTab : public QWidget
{
	Q_OBJECT

public:
	struct OverwritePass
	{
		KuuraFillType fillType;
		QByteArray fillValue;
		bool verify;
	};

	explicit AlgorithmTab(QWidget *parent = nullptr);
	~AlgorithmTab();

	const QVector<OverwritePass>& selectedPasses() const;

signals:
	void backRequested();
	void nextRequested();

private:
	void presetChanged(int index);

	Ui::AlgorithmTab *ui;
};

