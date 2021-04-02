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

	int rowCount(const QModelIndex& /*parent*/) const
	{
		return static_cast<int>(_drives.size());
	}

	int columnCount(const QModelIndex&) const
	{
		return 5;
	}

	QVariant data(const QModelIndex& index, int role) const
	{
		if (role == Qt::CheckStateRole && index.column() == 4)
		{
			return Qt::CheckState::Unchecked;
		}

		if (role == Qt::DisplayRole)
		{
			const size_t row = static_cast<size_t>(index.row());
			Q_ASSERT(row <= _drives.size());
			const Kuura::Drive& drive = _drives[row];

			switch (index.column())
			{
				case 0:
					return QString::fromStdString(drive.Path);
				case 1:
					return QString::fromStdString(drive.Description);
				case 2:
					return drive.Partitions;
				case 3:
					return QLocale().formattedDataSize(drive.Capacity);
				case 4:
					return "No";
			}

			qCritical() << index << " is out of bounds";
		}

		return QVariant();
	}

	QVariant headerData(int section, Qt::Orientation orientation, int role) const
	{
		if (role == Qt::DisplayRole)
		{
			if (orientation == Qt::Orientation::Horizontal)
			{
				switch (section)
				{
					case 0:
						return "Path";
					case 1:
						return "Description";
					case 2:
						return "Partitions";
					case 3:
						return "Size";
					case 4:
						return "Selected";
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

	void refresh()
	{
		beginResetModel();
		_drives = Kuura::DriveInfo();
		endResetModel();
	}

	QStringList selectedDrives() const
	{
		return {}; // TODO:
	}

private:
	std::vector<Kuura::Drive> _drives = Kuura::DriveInfo();
};

DriveSelectTab::DriveSelectTab(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::DriveSelectTab)
{
	ui->setupUi(this);

	auto model = new DriveSelectModel(this);
	ui->tableViewDrives->setModel(model);

	connect(ui->pushButtonBack, &QPushButton::clicked, [this]()
	{
		emit backRequested();
	});

	connect(ui->pushButtonNext, &QPushButton::clicked, [this]()
	{
		emit nextRequested();
	});

	connect(ui->pushButtonRefresh, &QPushButton::clicked, model, &DriveSelectModel::refresh);
}

DriveSelectTab::~DriveSelectTab()
{
	delete ui;
	qDebug();
}

QStringList DriveSelectTab::selectedDrives() const
{
	auto model = reinterpret_cast<DriveSelectModel*>(ui->tableViewDrives->model());
	Q_ASSERT(model);
	return model->selectedDrives();
}
