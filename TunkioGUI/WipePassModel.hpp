#pragma once

#include <QAbstractTableModel>

class WipePassModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	WipePassModel(QObject* parent = nullptr);
	virtual ~WipePassModel();

	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};
