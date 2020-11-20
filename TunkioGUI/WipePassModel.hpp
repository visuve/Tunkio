#pragma once

#include <QAbstractTableModel>
#include <QTime>

#include <mutex>

#include "../TunkioAPI/TunkioAPI.h"

class WipePassModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	WipePassModel(QObject* parent = nullptr);
	virtual ~WipePassModel();

	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

public slots:
	void onPassAdded(TunkioFillType fillType, const QString& fillValue, bool verify);
	void onWipeStarted(uint16_t passes, uint64_t bytesToWritePerPass);
	void onPassStarted(uint16_t pass);
	void onPassProgressed(uint16_t pass, uint64_t bytesWritten);
	void onPassFinished(uint16_t pass);
	void onWipeCompleted(uint16_t passes, uint64_t totalBytesWritten);

private:

	struct Pass
	{
		TunkioFillType fillType;
		QString fillValue;
		bool verify;
		uint64_t bytesWritten;
		uint64_t bytesToWrite;
		QTime start;
		QTime finish;
	};

	QList<Pass> m_passes;
	std::mutex m_mutex;
};
