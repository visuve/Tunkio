#pragma once

#include <QStyledItemDelegate>

class ProgressBarDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	ProgressBarDelegate(QObject* parent = nullptr);

	void paint(QPainter* painter,
		const QStyleOptionViewItem& option,
		const QModelIndex& index) const;

private:
	int m_column = 0;
};