#include "KuuraGUI-PCH.hpp"
#include "ProgressBarDelegate.hpp"

namespace
{
	constexpr QColor PowderBlue(176, 224, 230);
	constexpr QColor Crystalsong(79, 179, 179);
	const QBrush Light = QApplication::palette().light();
}

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

	painter->fillRect(option.rect, Light);

	const float percent = std::clamp(data.toFloat(), 0.00f, 100.00f);

	QRect progressRect = option.rect;
	progressRect.setWidth(progressRect.width() * (percent / 100.0f ));

	painter->setPen(Qt::NoPen);
	painter->fillRect(progressRect, PowderBlue);

	QBrush verBrush(Crystalsong, index.parent().isValid() ? Qt::BDiagPattern : Qt::FDiagPattern);
	painter->setBrush(verBrush);
	painter->drawRect(progressRect);
}
