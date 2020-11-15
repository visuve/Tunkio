#include "DriveSelectDialog.hpp"
#include "ui_DriveSelectDialog.h"
#include "TunkioDriveInfo.hpp"
#include <QDebug>
#include <QLocale>
#include <QMessageBox>
#include <QPushButton>

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

			switch (index.column())
			{
				case 0:
					return QString::fromStdString(m_drives[row].Path);
				case 1:
					return QString::fromStdString(m_drives[row].Description);
				case 2:
					return m_drives[row].Partitions;
				case 3:
					// TODO: maybe this should be asked from qApp?
					return QLocale().formattedDataSize(m_drives[row].Capacity);
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
	std::vector<Tunkio::Drive> m_drives = Tunkio::DriveInfo();
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
				"Tunkio - Please select a drive",
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
