#pragma once

#include <QAbstractItemModel>

namespace Ui
{
	const QTime ZeroTime(0, 0, 0, 0);

	const QLocale SystemLocale = QLocale::system();

	inline QString toQString(const std::filesystem::path& path)
	{
		return QString::fromStdString(path.string());
	}

	inline QString millisecondsToString(float milliseconds)
	{
		return SystemLocale.toString(ZeroTime.addMSecs(milliseconds), QLocale::LongFormat);
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
	ProgressModel(const QVariantList& headerData, QObject* parent = nullptr);
	~ProgressModel();

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	QModelIndex index(int row, int column, const QModelIndex& parentIndex = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex& childIndex) const override;

	int rowCount(const QModelIndex& parentIndex = QModelIndex()) const override;
	int columnCount(const QModelIndex&) const override;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

	void addTarget(const QVariant& path);
	void addPass(const QVariantList& data);
	bool setData(const QString& path, int row, int column, const QVariant& value);
	// QVariantList* data(const QString& path, int row, int column);

private:
	ProgressNode* _root;
};
