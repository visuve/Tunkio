#include "ProgressBarDelegate.hpp"
#include <QApplication>

ProgressBarDelegate::ProgressBarDelegate(QObject* parent) :
	QStyledItemDelegate(parent)
{
}

void ProgressBarDelegate::paint(
	QPainter* painter,
	const QStyleOptionViewItem& option,
	const QModelIndex& index) const
{
	double progress = index.model()->data(index, Qt::DisplayRole).toDouble();

	QStyleOptionProgressBar progressBar;
	progressBar.rect = option.rect;
	progressBar.minimum = 0;
	progressBar.maximum = 100;
	progressBar.progress = progress;
	progressBar.text = QString::number(progress, 'g', 2);
	progressBar.textVisible = true;
	
	QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBar, painter);
}