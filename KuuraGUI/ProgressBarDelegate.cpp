#include "KuuraGUI-PCH.hpp"
#include "ProgressBarDelegate.hpp"

ProgressBarDelegate::ProgressBarDelegate(QObject* parent) :
	QStyledItemDelegate(parent)
{
}

ProgressBarDelegate::~ProgressBarDelegate()
{
	qDebug();
}

void ProgressBarDelegate::paint(
	QPainter* painter,
	const QStyleOptionViewItem& option,
	const QModelIndex& index) const
{
	if (!index.isValid())
	{
		return;
	}

	const QVariant data = index.model()->data(index, Qt::DisplayRole);

	if (!data.isValid())
	{
		return;
	}

	float percent = std::clamp(data.toFloat(), 0.00f, 100.00f);

	QRect progressRect = option.rect;
	progressRect.setWidth(progressRect.width() * (percent / 100.0f ));

	painter->setPen(Qt::NoPen);
	painter->fillRect(progressRect, Qt::green);

	QBrush verBrush(Qt::darkGreen, index.parent().isValid() ? Qt::BDiagPattern : Qt::FDiagPattern);
	painter->setBrush(verBrush);
	painter->drawRect(progressRect);
}
