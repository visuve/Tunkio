#include "KuuraGUI-PCH.hpp"
#include "DriveSelectTab.hpp"
#include "ui_DriveSelectTab.h"
#include "../KuuraLIB/KuuraDriveInfo.hpp"

class DriveSelectModel : public QAbstractTableModel
{
public:
	DriveSelectModel(QObject* parent) :
		QAbstractTableModel(parent)
	{
	}

	~DriveSelectModel()
	{
		qDebug();
	}

	int rowCount(const QModelIndex& /*parent*/) const override
	{
		return _drives.size();
	}

	int columnCount(const QModelIndex&) const override
	{
		return 5;
	}

	QVariant data(const QModelIndex& index, int role) const override
	{
		const int row = index.row();
		Q_ASSERT(row <= _drives.size());
		const auto& drive = _drives[row];

		if (role == Qt::CheckStateRole && index.column() == 0)
		{
			return drive.second ? Qt::Checked : Qt::Unchecked;
		}

		if (role == Qt::DisplayRole)
		{
			switch (index.column())
			{
				case 0:
					return drive.second ? "Yes" : "No";
				case 1:
					return QString::fromStdString(drive.first.Path);
				case 2:
					return QString::fromStdString(drive.first.Description);
				case 3:
					return drive.first.Partitions;
				case 4:
					return QLocale().formattedDataSize(drive.first.Capacity);
			}

			qCritical() << index << " is out of bounds";
		}

		return QVariant();
	}

	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override
	{
		if (role == Qt::CheckStateRole && index.column() == 0)
		{
			const int row = index.row();
			Q_ASSERT(row <= _drives.size());
			_drives[row].second = value.toBool();
			emit dataChanged(index, index);
			return true;
		}

		return false;
	}

	QVariant headerData(int section, Qt::Orientation orientation, int role) const override
	{
		if (role == Qt::DisplayRole)
		{
			if (orientation == Qt::Orientation::Horizontal)
			{
				switch (section)
				{
					case 0:
						return "Selected";
					case 1:
						return "Path";
					case 2:
						return "Description";
					case 3:
						return "Partitions";
					case 4:
						return "Size";
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

	Qt::ItemFlags flags(const QModelIndex& index) const override
	{
		Qt::ItemFlags existing = QAbstractTableModel::flags(index);

		if (index.column() == 0)
		{
			return existing | Qt::ItemIsUserCheckable;
		}

		return existing;
	}

	void refresh()
	{
		beginResetModel();

		_drives.clear();

		for (auto& drive : Kuura::DriveInfo())
		{
			QPair<Kuura::Drive, bool> pair(drive, false);
			_drives.emplaceBack(pair);
		}

		endResetModel();
	}

	QStringList selectedDrives() const
	{
		QStringList selected;

		for (const auto& drive : _drives)
		{
			if (!drive.second)
			{
				continue;
			}

			selected.emplaceBack(QString::fromStdString(drive.first.Path));
		}

		return selected;
	}

private:
	QVector<QPair<Kuura::Drive, bool>> _drives;
};

DriveSelectTab::DriveSelectTab(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::DriveSelectTab)
{
	ui->setupUi(this);

	ui->tableViewDrives->setModel(new DriveSelectModel(this));

	connect(ui->pushButtonBack, &QPushButton::clicked, [this]()
	{
		emit backRequested();
	});

	connect(ui->pushButtonNext, &QPushButton::clicked, [this]()
	{
		emit nextRequested();
	});

	connect(ui->pushButtonRefresh, &QPushButton::clicked, this, &DriveSelectTab::refreshDrives);
}

DriveSelectTab::~DriveSelectTab()
{
	delete ui;
	qDebug();
}

void DriveSelectTab::refreshDrives()
{
	auto model = reinterpret_cast<DriveSelectModel*>(ui->tableViewDrives->model());
	Q_ASSERT(model);
	model->refresh();
}

QStringList DriveSelectTab::selectedDrives() const
{
	auto model = reinterpret_cast<DriveSelectModel*>(ui->tableViewDrives->model());
	Q_ASSERT(model);
	return model->selectedDrives();
}
