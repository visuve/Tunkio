#pragma once

#include <QAbstractItemModel>

namespace Ui
{
	using MilliSeconds = std::chrono::duration<float, std::ratio<1, 1000>>;

	const QTime ZeroTime(0, 0, 0, 0);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	const QLocale SystemLocale = QLocale::system();
#else
	QLocale SystemLocale = QLocale::system();
#endif

	inline QString toQString(const std::filesystem::path& path)
	{
		return QString::fromStdString(path.string());
	}

	inline QString millisecondsToString(float milliseconds)
	{
		return SystemLocale.toString(ZeroTime.addMSecs(milliseconds), QLocale::LongFormat);
	}

	inline QString millisecondsToString(MilliSeconds milliseconds)
	{
		return millisecondsToString(milliseconds.count());
	}

	inline QString bytesToString(float bytes)
	{
		return SystemLocale.formattedDataSize(bytes);
	}
}


class ProgressNode;

class ProgressModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	ProgressModel(QObject* parent = nullptr);
	~ProgressModel();

	QModelIndex index(int row, int column, const QModelIndex& parentIndex = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex& childIndex) const override;
	int rowCount(const QModelIndex& parentIndex = QModelIndex()) const override;
	int columnCount(const QModelIndex&) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	void addNode(const std::filesystem::path& path);
	void setPassCount(uint16_t passes);
	std::pair<int, ProgressNode*> findTopLevelNode(const std::filesystem::path& path);
	std::pair<int, ProgressNode*> findProgressNode(const std::filesystem::path& path, uint16_t pass);
	void beginTarget(const std::filesystem::path& path, uint64_t bytesToWrite);
	void updateNode(const std::filesystem::path& path, uint16_t pass, uint64_t bytesWritten);

	float bytesWrittenTotal() const;

private:
	ProgressNode* _root;
};
