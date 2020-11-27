#include "TunkioGUI-PCH.hpp"
#include "ProgressBarDelegate.hpp"

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

	if (progress <= 0.00)
	{
		progressBar.text = "0%";
	}
	else if (progress >= 99.99)
	{
		progressBar.text = "100%";
	}
	else
	{
		progressBar.text = QString::number(progress, 'g', 2) + '%';
	}

	progressBar.textVisible = true;
	
	QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBar, painter);
}
