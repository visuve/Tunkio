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
	float progress = index.model()->data(index, Qt::DisplayRole).toFloat();

	QStyleOptionProgressBar progressBar;
	progressBar.rect = option.rect;
	progressBar.minimum = 0;
	progressBar.maximum = 100;
	progressBar.progress = static_cast<int>(progress);
	progressBar.textVisible = true;

	if (progress <= 0.00f)
	{
		progressBar.text = "0%";
	}
	else if (progress >= 99.99f)
	{
		progressBar.text = "100%";
	}
	else
	{
		progressBar.text = QLocale::system().toString(progress, 'f', 2) + '%';
	}

	QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBar, painter);
}
