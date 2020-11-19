#include "WipePassModel.hpp"
#include <QDebug>
#include <QTime>

QString toString(TunkioFillType type)
{
	switch (type)
	{
		case TunkioFillType::Zeroes:
			return "Zeroes";
		case TunkioFillType::Ones:
			return "Ones";
		case TunkioFillType::Character:
			return "Character";
		case TunkioFillType::Sentence:
			return "Sentence";
		case TunkioFillType::Random:
			return "Random";
	}

	qCritical() << int(type) << " is out of bounds";
	return "Unknown?";
}

WipePassModel::WipePassModel(QObject* parent) :
	QAbstractTableModel(parent)
{
}

WipePassModel::~WipePassModel()
{
	m_passes.clear();
}

int WipePassModel::rowCount(const QModelIndex& /*parent*/) const
{
	return m_passes.count();
}

int WipePassModel::columnCount(const QModelIndex&) const
{
	return 8;
}

QVariant WipePassModel::data(const QModelIndex& index, int role) const
{
	if (role == Qt::DisplayRole)
	{
		const int row = index.row();
		Q_ASSERT(row <= m_passes.size());
		const Pass& pass = m_passes[row];

		switch (index.column())
		{
			case 0:
				return toString(pass.fillType);
			case 1:
				return pass.fillValue;
			case 2:
				return pass.verify ? "Yes" : "No";
			case 3:
			{
				if (!pass.start.isValid())
				{
					break;
				}

				return QLocale().formattedDataSize(pass.bytesWritten);
			}
			case 4:
			{
				if (!pass.start.isValid())
				{
					break;
				}

				return QLocale().formattedDataSize(pass.bytesToWrite - pass.bytesWritten);
			}
			case 5:
			{
				if (!pass.start.isValid())
				{
					break;
				}

				int64_t secondsTaken = pass.start.secsTo(QTime::currentTime());
				int64_t bytesPerSecond = pass.bytesWritten / secondsTaken;
				return QLocale().formattedDataSize(bytesPerSecond).append("/s");
			}
			case 6:
			{
				if (!pass.start.isValid())
				{
					break;
				}

				int64_t secondsTaken = pass.start.secsTo(QTime::currentTime());
				int64_t bytesPerSecond = pass.bytesWritten / secondsTaken;
				int64_t bytesLeft = pass.bytesToWrite - pass.bytesWritten;
				int64_t secondsLeft = bytesLeft / bytesPerSecond;

				QTime timeLeft(0, 0, 0);
				timeLeft = timeLeft.addSecs(secondsLeft);
				return timeLeft.toString(Qt::ISODate);
			}
			case 7:
			{
				if (!pass.start.isValid())
				{
					break;
				}

				return double(pass.bytesWritten) / double(pass.bytesToWrite) * 100;
			}

			default:
				qCritical() << index << " is out of bounds";
		}
	}

	return QVariant();
}

bool WipePassModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (role == Qt::EditRole)
	{
		const int row = index.row();
		Q_ASSERT(row <= m_passes.size());
		Pass& pass = m_passes[row];

		switch (index.column())
		{
			case 0:
				pass.fillType = static_cast<TunkioFillType>(value.toInt());
				return true;
			case 1:
				pass.fillValue = value.toString();
				return true;
			case 2:
				pass.verify = value.toBool();
				return true;
		}

		qCritical() << index << " is not editable";
	}

	return false;
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
					return "Verify";
				case 3:
					return "Written";
				case 4:
					return "Left";
				case 5:
					return "Speed";
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

void WipePassModel::onPassAdded(TunkioFillType fillType, const QString& fillValue, bool verify)
{
	int row = static_cast<int>(m_passes.size());
	beginInsertRows(QModelIndex(), row, row);

	Pass pass = {};
	pass.fillType = fillType;
	pass.fillValue = fillValue;
	pass.verify = verify;
	m_passes.append(pass);

	endInsertRows();
}

void WipePassModel::onWipeStarted(uint16_t totalIterations, uint64_t bytesToWritePerIteration)
{
	Q_ASSERT(totalIterations == m_passes.size());

	for (Pass& pass : m_passes)
	{
		pass.bytesToWrite = bytesToWritePerIteration;
	}
}


void WipePassModel::onPassStarted(uint16_t pass)
{
	Q_ASSERT(pass <= m_passes.size());
	int row = pass - 1;

	m_passes[row].start = QTime::currentTime();

	auto tl = index(row, 3);
	auto br = index(row, 7);
	emit dataChanged(tl, br, { Qt::DisplayRole });
}

void WipePassModel::onPassProgressed(uint16_t pass, uint64_t bytesWritten)
{
	Q_ASSERT(pass <= m_passes.size());
	int row = pass - 1;

	m_passes[row].bytesWritten = bytesWritten;

	auto tl = index(row, 3);
	auto br = index(row, 7);
	emit dataChanged(tl, br, { Qt::DisplayRole });
}

void WipePassModel::onPassFinished(uint16_t pass)
{
	Q_ASSERT(pass <= m_passes.size());
	int row = pass - 1;

	m_passes[row].finish = QTime::currentTime();

	auto tl = index(row, 3);
	auto br = index(row, 7);
	emit dataChanged(tl, br, { Qt::DisplayRole });
}

void WipePassModel::onWipeCompleted(uint16_t, uint64_t)
{
	// TODO: show stats
}
