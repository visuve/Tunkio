#include "KuuraGUI-PCH.hpp"
#include "OverwritePassModel.hpp"

namespace Ui
{
	const QTime ZeroTime(0, 0, 0, 0);

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	const QLocale SystemLocale = QLocale::system();
#else
	QLocale SystemLocale = QLocale::system(); // Hack to satisfy Clang & GCC atm
#endif

	QString fillTypeToString(KuuraFillType type)
	{
		switch (type)
		{
			case KuuraFillType::ZeroFill:
				return "Zeroes";
			case KuuraFillType::OneFill:
				return "Ones";
			case KuuraFillType::ByteFill:
				return "Byte";
			case KuuraFillType::SequenceFill:
				return "Sequence";
			case KuuraFillType::FileFill:
				return "File";
			case KuuraFillType::RandomFill:
				return "Random";
		}

		qCritical() << int(type) << " is out of bounds";
		return "Unknown?";
	}

	QVariant fillValue(const OverwritePassModel::Pass& pass)
	{
		switch (pass.fillType)
		{
			case KuuraFillType::FileFill:
				return QString(pass.fillValue);
			case KuuraFillType::RandomFill:
				return "Random";
			default:
				return pass.fillValue.toHex(' ');
		}
	}

	QVariant bytesWritten(const OverwritePassModel::Pass& pass)
	{
		return SystemLocale.formattedDataSize(pass.bytesWritten);
	}

	QVariant bytesLeft(const OverwritePassModel::Pass& pass)
	{
		return SystemLocale.formattedDataSize(pass.bytesToWrite - pass.bytesWritten);
	}

	QVariant speedToString(const OverwritePassModel::Pass& pass)
	{
		int64_t milliSecondsTaken = pass.start.msecsTo(pass.time);

		if (milliSecondsTaken <= 0 || pass.bytesWritten <= 0)
		{
			return QVariant();
		}

		int64_t bytesPerSecond = pass.bytesWritten * 1000 / milliSecondsTaken;
		return SystemLocale.formattedDataSize(bytesPerSecond).append("/s");
	}

	QVariant timeTaken(const OverwritePassModel::Pass& pass)
	{
		int64_t milliSecondsTaken = pass.start.msecsTo(pass.time);

		if (milliSecondsTaken <= 0)
		{
			return QVariant();
		}

		QTime timeLeft = ZeroTime.addMSecs(milliSecondsTaken);
		return SystemLocale.toString(timeLeft, QLocale::LongFormat);
	}

	QVariant timeLeft(const OverwritePassModel::Pass& pass)
	{
		int64_t milliSecondsTaken = pass.start.msecsTo(pass.time);

		if (milliSecondsTaken <= 0 || pass.bytesWritten <= 0)
		{
			return QVariant();
		}

		int64_t bytesPerSecond = pass.bytesWritten * 1000 / milliSecondsTaken;
		int64_t bytesLeft = pass.bytesToWrite - pass.bytesWritten;
		int64_t secondsLeft = bytesLeft / bytesPerSecond;

		QTime timeLeft = ZeroTime.addSecs(secondsLeft);
		return SystemLocale.toString(timeLeft, QLocale::LongFormat);
	}

	float progressPercent(const OverwritePassModel::Pass& pass)
	{
		if (pass.bytesWritten <= 0)
		{
			return 0;
		}

		if (pass.bytesToWrite <= 0)
		{
			return 100;
		}

		return float(pass.bytesWritten) / float(pass.bytesToWrite) * 100.0f;
	}
}

OverwritePassModel::OverwritePassModel(QObject* parent) :
	QAbstractTableModel(parent)
{
}

OverwritePassModel::~OverwritePassModel()
{
	m_passes.clear();
}

int OverwritePassModel::rowCount(const QModelIndex& /*parent*/) const
{
	return m_passes.count();
}

int OverwritePassModel::columnCount(const QModelIndex&) const
{
	return 9;
}

QVariant OverwritePassModel::data(const QModelIndex& index, int role) const
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
				return Ui::fillValue(pass);
			case 2:
				return pass.verify ? "Yes" : "No";
			case 3:
			{
				if (!pass.start.isValid())
				{
					break;
				}

				return Ui::bytesWritten(pass);
			}
			case 4:
			{
				if (!pass.start.isValid())
				{
					break;
				}

				return Ui::bytesLeft(pass);
			}
			case 5:
			{
				if (!pass.start.isValid())
				{
					break;
				}

				return Ui::timeTaken(pass);
			}
			case 6:
			{
				if (!pass.start.isValid())
				{
					break;
				}

				return Ui::timeLeft(pass);
			}
			case 7:
			{
				if (!pass.start.isValid())
				{
					break;
				}

				return Ui::speedToString(pass);
			}
			case 8:
			{
				if (!pass.start.isValid())
				{
					break;
				}

				return Ui::progressPercent(pass);
			}
			default:
				qCritical() << index << " is out of bounds";
		}
	}

	return QVariant();
}

bool OverwritePassModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (role == Qt::EditRole)
	{
		const int row = index.row();
		Q_ASSERT(row <= m_passes.size());
		Pass& pass = m_passes[row];

		switch (index.column())
		{
			case 0:
				pass.fillType = static_cast<KuuraFillType>(value.toInt());
				return true;
			case 1:
				pass.fillValue = value.toByteArray();
				return true;
			case 2:
				pass.verify = value.toBool();
				return true;
		}

		qCritical() << index << " is not editable";
	}

	return false;
}

QVariant OverwritePassModel::headerData(int section, Qt::Orientation orientation, int role) const
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

void OverwritePassModel::addPass(KuuraFillType fillType, bool verify, const QByteArray& fillValue)
{
	int row = static_cast<int>(m_passes.size());
	beginInsertRows(QModelIndex(), row, row);

	Pass pass = {};
	pass.fillType = fillType;

	switch (fillType)
	{
		case ZeroFill:
		{
			Q_ASSERT(fillValue.isEmpty());
			pass.fillValue = QByteArray("\x00", 1);
			break;
		}
		case OneFill:
		{
			Q_ASSERT(fillValue.isEmpty());
			pass.fillValue = QByteArray("\xFF", 1);
			break;
		}
		case ByteFill:
		{
			Q_ASSERT(fillValue.size() == 1);
			pass.fillValue = fillValue;
			break;
		}
		case RandomFill:
		{
			Q_ASSERT(fillValue.isEmpty());
			break;
		}
		default:
			pass.fillValue = fillValue;
			break;
	}

	pass.verify = verify;
	m_passes.append(pass);
	endInsertRows();
}

bool OverwritePassModel::isEmpty() const
{
	return m_passes.isEmpty();
}

const QList<OverwritePassModel::Pass>& OverwritePassModel::passes() const
{
	return m_passes;
}

void OverwritePassModel::clearStats()
{
	for (Pass& pass : m_passes)
	{
		pass.bytesWritten = 0;
		pass.bytesToWrite = 0;
		pass.start = QTime();
		pass.start = QTime();
	}
}

void OverwritePassModel::clearPasses()
{
	if (!m_passes.isEmpty())
	{
		beginResetModel();
		m_passes.clear();
		endResetModel();
	}
}

void OverwritePassModel::removePass(int row)
{
	Q_ASSERT(row < m_passes.size());
	beginRemoveRows(QModelIndex(), row, row);
	m_passes.removeAt(row);
	endRemoveRows();
}

void OverwritePassModel::onOverwriteStarted(uint16_t passes, uint64_t bytesToWritePerPass)
{
	Q_ASSERT(passes == m_passes.size());

	for (Pass& pass : m_passes)
	{
		pass.bytesToWrite = bytesToWritePerPass;
	}

	qDebug() << "Overwrite started:" << passes << '/' << bytesToWritePerPass;
}


void OverwritePassModel::onPassStarted(const std::filesystem::path& path, uint16_t pass)
{
	rowData(pass).start = QTime::currentTime();
	updateRow(pass);

	qDebug() << "Pass started:" << path << pass;
}

void OverwritePassModel::onPassProgressed(const std::filesystem::path&, uint16_t pass, uint64_t bytesWritten)
{
	Pass& current = rowData(pass);
	current.bytesWritten = bytesWritten;
	current.time = QTime::currentTime();
	updateRow(pass);
}

void OverwritePassModel::onPassFinished(const std::filesystem::path& path, uint16_t pass)
{
	rowData(pass).time = QTime::currentTime();
	updateRow(pass);

	qDebug() << "Pass finished:" << path << pass;
}

void OverwritePassModel::onOverwriteCompleted(uint16_t pass, uint64_t totalBytesWritten)
{
	Q_ASSERT(pass <= m_passes.size());
	qDebug() << "Overwrite finished:" << pass << '/' << totalBytesWritten;
}

OverwritePassModel::Pass& OverwritePassModel::rowData(uint16_t pass)
{
	Q_ASSERT(pass <= m_passes.size());
	int row = pass - 1;
	return m_passes[row];
}

void OverwritePassModel::updateRow(uint16_t pass)
{
	int row = pass - 1;
	const QModelIndex topLeft = index(row, 3);
	const QModelIndex bottomRight = index(row, 8);

	emit dataChanged(topLeft, bottomRight, { Qt::DisplayRole });
}
