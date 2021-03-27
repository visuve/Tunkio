#include "KuuraGUI-PCH.hpp"
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
	const QVariant data = index.model()->data(index, Qt::DisplayRole);

	if (!data.isValid())
	{
		return;
	}

	float progress = data.toFloat();

	QStyleOptionProgressBar progressBar;
	progressBar.rect = option.rect;
	progressBar.minimum = 0;
	progressBar.maximum = 100;
	progressBar.textVisible = true;
	progressBar.textAlignment = Qt::AlignmentFlag::AlignCenter;

	if (progress <= 0.00f)
	{
		progressBar.progress = 0;
		progressBar.text = "0%";
	}
	else if (progress >= 99.99f)
	{
		progressBar.progress = 100;
		progressBar.text = "100%";
	}
	else
	{
		progressBar.progress = static_cast<int>(progress);
		progressBar.text = QLocale::system().toString(progress, 'f', 2) + '%';
	}

	QApplication::style()->drawControl(QStyle::CE_ProgressBarContents, &progressBar, painter);
}
