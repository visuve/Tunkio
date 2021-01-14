#include "KuuraGUI-PCH.hpp"
#include "DriveSelectDialog.hpp"
#include "ui_DriveSelectDialog.h"
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
		m_drives.clear();
	}

	int rowCount(const QModelIndex& /*parent*/) const
	{
		return static_cast<int>(m_drives.size());
	}

	int columnCount(const QModelIndex&) const
	{
		return 4;
	}

	QVariant data(const QModelIndex& index, int role) const
	{
		if (role == Qt::DisplayRole)
		{
			const size_t row = static_cast<size_t>(index.row());
			Q_ASSERT(row <= m_drives.size());
			const Kuura::Drive& drive = m_drives[row];

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

private:
	std::vector<Kuura::Drive> m_drives = Kuura::DriveInfo();
};


DriveSelectDialog::DriveSelectDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::DriveSelectDialog())
{
	ui->setupUi(this);

	auto openButton = ui->buttonBoxDriveSelect->button(QDialogButtonBox::Open);
	auto cancelButton = ui->buttonBoxDriveSelect->button(QDialogButtonBox::Cancel);

	connect(openButton, &QPushButton::clicked, this, [this]
	{
		QItemSelectionModel* selectionModel = ui->tableViewDrives->selectionModel();

		if (!selectionModel->hasSelection())
		{
			QMessageBox::information(
				this,
				"Kuura - Please select a drive",
				"No drive has been selected.\nPlease select one, or click Cancel to return.");
			return;
		}

		return QDialog::accept();
	});
	connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

	ui->tableViewDrives->setModel(new DriveSelectModel(this));
}

DriveSelectDialog::~DriveSelectDialog()
{
	delete ui;
}

QString DriveSelectDialog::selectedDrive()
{
	const QItemSelectionModel* selectionModel = ui->tableViewDrives->selectionModel();

	if (!selectionModel->hasSelection())
	{
		return {};
	}

	const QModelIndex selected = selectionModel->selectedRows(0).first();
	return ui->tableViewDrives->model()->data(selected).toString();
}
