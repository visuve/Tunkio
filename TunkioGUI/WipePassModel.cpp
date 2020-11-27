#include "TunkioGUI-PCH.hpp"
#include "WipePassModel.hpp"

namespace
{
	QLocale g_locale;
}

QString Ui::fillTypeToString(TunkioFillType type)
{
	switch (type)
	{
		case TunkioFillType::ZeroFill:
			return "Zeroes";
		case TunkioFillType::OneFill:
			return "Ones";
		case TunkioFillType::CharacterFill:
			return "Character";
		case TunkioFillType::SentenceFill:
			return "Sentence";
		case TunkioFillType::FileFill:
			return "File";
		case TunkioFillType::RandomFill:
			return "Random";
	}

	qCritical() << int(type) << " is out of bounds";
	return "Unknown?";
}

QVariant speedToString(const WipePassModel::Pass& pass)
{
	int64_t milliSecondsTaken = pass.start.msecsTo(pass.time);

	if (milliSecondsTaken <= 0 || pass.bytesWritten <= 0)
	{
		return QVariant();
	}

	int64_t bytesPerSecond = pass.bytesWritten * 1000 / milliSecondsTaken;
	return g_locale.formattedDataSize(bytesPerSecond).append("/s");
}

QVariant timeTaken(const WipePassModel::Pass& pass)
{
	int64_t milliSecondsTaken = pass.start.msecsTo(pass.time);

	if (milliSecondsTaken <= 0)
	{
		return QVariant();
	}

	QTime timeLeft(0, 0, 0);
	timeLeft = timeLeft.addMSecs(milliSecondsTaken);
	return timeLeft.toString(Qt::ISODate);
}

QVariant timeLeft(const WipePassModel::Pass& pass)
{
	int64_t milliSecondsTaken = pass.start.msecsTo(pass.time);

	if (milliSecondsTaken <= 0 || pass.bytesWritten <= 0)
	{
		return QVariant();
	}

	int64_t bytesPerSecond = pass.bytesWritten * 1000 / milliSecondsTaken;
	int64_t bytesLeft = pass.bytesToWrite - pass.bytesWritten;
	int64_t secondsLeft = bytesLeft / bytesPerSecond;

	QTime timeLeft(0, 0, 0);
	timeLeft = timeLeft.addSecs(secondsLeft);
	return timeLeft.toString(Qt::ISODate);
}

double progressPercent(const WipePassModel::Pass& pass)
{
	if (pass.bytesWritten <= 0)
	{
		return 0;
	}

	if (pass.bytesToWrite <= 0)
	{
		return 100;
	}

	return double(pass.bytesWritten) / double(pass.bytesToWrite) * 100;
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
	return 9;
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
				return Ui::fillTypeToString(pass.fillType);
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

				return g_locale.formattedDataSize(pass.bytesWritten);
			}
			case 4:
			{
				if (!pass.start.isValid())
				{
					break;
				}

				return g_locale.formattedDataSize(pass.bytesToWrite - pass.bytesWritten);
			}
			case 5:
			{
				if (!pass.start.isValid())
				{
					break;
				}

				return timeTaken(pass);
			}
			case 6:
			{
				if (!pass.start.isValid())
				{
					break;
				}

				return timeLeft(pass);
			}
			case 7:
			{
				if (!pass.start.isValid())
				{
					break;
				}

				return speedToString(pass);
			}
			case 8:
			{
				if (!pass.start.isValid())
				{
					break;
				}

				return progressPercent(pass);
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
					return "Bytes Written";
				case 4:
					return "Bytes Left";
				case 5:
					return "Time Taken";
				case 6:
					return "Time Left";
				case 7:
					return "Speed";
				case 8:
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

void WipePassModel::addPass(TunkioFillType fillType, const QString& fillValue, bool verify)
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

bool WipePassModel::isEmpty() const
{
	return m_passes.isEmpty();
}

const QList<WipePassModel::Pass>& WipePassModel::passes() const
{
	return m_passes;
}

void WipePassModel::removePass(int row)
{
	Q_ASSERT(row < m_passes.size());
	beginRemoveRows(QModelIndex(), row, row);
	m_passes.removeAt(row);
	endRemoveRows();
}

void WipePassModel::onWipeStarted(uint16_t passes, uint64_t bytesToWritePerPass)
{
	Q_ASSERT(passes == m_passes.size());

	for (Pass& pass : m_passes)
	{
		pass.bytesToWrite = bytesToWritePerPass;
	}

	qDebug() << "Wipe started:" << passes << '/' << bytesToWritePerPass;
}


void WipePassModel::onPassStarted(uint16_t pass)
{
	Q_ASSERT(pass <= m_passes.size());
	int row = pass - 1;

	m_passes[row].start = QTime::currentTime();
	updateRow(row);

	qDebug() << "Pass started:" << pass << "row:" << row;
}

void WipePassModel::onPassProgressed(uint16_t pass, uint64_t bytesWritten)
{
	Q_ASSERT(pass <= m_passes.size());
	int row = pass - 1;

	m_passes[row].bytesWritten = bytesWritten;
	m_passes[row].time = QTime::currentTime();
	updateRow(row);
}

void WipePassModel::onPassFinished(uint16_t pass)
{
	Q_ASSERT(pass <= m_passes.size());
	int row = pass - 1;

	m_passes[row].time = QTime::currentTime();
	updateRow(row);

	qDebug() << "Pass finished:" << pass << "row:" << row;
}

void WipePassModel::onWipeCompleted(uint16_t pass, uint64_t totalBytesWritten)
{
	Q_ASSERT(pass <= m_passes.size());
	qDebug() << "Wipe finished:" << pass << '/' << totalBytesWritten;
}

void WipePassModel::updateRow(int row)
{
	const QModelIndex topLeft = index(row, 3);
	const QModelIndex bottomRight = index(row, 8);
	emit dataChanged(topLeft, bottomRight, { Qt::DisplayRole });
}
