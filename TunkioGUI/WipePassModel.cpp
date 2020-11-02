#include "WipePassModel.hpp"
#include <QDebug>
#include <QTime>

WipePassModel::WipePassModel(QObject* parent) :
	QAbstractTableModel(parent)
{
}

WipePassModel::~WipePassModel()
{
}

int WipePassModel::rowCount(const QModelIndex& /*parent*/) const
{
	return 1;
}

int WipePassModel::columnCount(const QModelIndex&) const
{
	return 8;
}

QVariant WipePassModel::data(const QModelIndex& index, int role) const
{
	if (role == Qt::DisplayRole)
	{
		switch (index.column())
		{
			case 0:
				return "Zeroes";
			case 1:
				return "0x00";
			case 2:
				return 12345678;
			case 3:
				return 123456789;
			case 4:
				return "99,78Mb/s";
			case 5:
				return "98,31Mb/s";
			case 6:
				return QTime(1, 2, 3, 4).toString(Qt::ISODate);
			case 7:
				return double(12345678) / double(123456789) * 100;
		}

		qCritical() << index << " is out of bounds";

	}

	return QVariant();
}

QVariant WipePassModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole)
	{
		if (orientation == Qt::Orientation::Horizontal)
		{
			switch (section)
			{
				case 0:
					return "Fill Type";
				case 1:
					return "Fill Value";
				case 2:
					return "Bytes Written";
				case 3:
					return "Bytes Left";
				case 4:
					return "Current Speed";
				case 5:
					return "Avg. Speed";
				case 6:
					return "Time Left";
				case 7:
					return "Progress";
			}

			qCritical() << section << " is out of bounds";
		}
		else
		{
			return ++section;
		}

	}

	return QVariant();
}