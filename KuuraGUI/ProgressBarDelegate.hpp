#pragma once

#include <QStyledItemDelegate>

class ProgressBarDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	explicit ProgressBarDelegate(QObject* parent = nullptr);
	~ProgressBarDelegate();

	void paint(QPainter* painter,
		const QStyleOptionViewItem& option,
		const QModelIndex& index) const;
};
