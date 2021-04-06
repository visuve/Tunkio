#include "KuuraGUI-PCH.hpp"
#include "PathSelectTab.hpp"
#include "ui_PathSelectTab.h"

class PathSelectModel : public QAbstractTableModel
{
public:
	PathSelectModel(QObject* parent) :
		QAbstractTableModel(parent)
	{
	}

	~PathSelectModel()
	{
		qDebug();
	}

	int rowCount(const QModelIndex&) const override
	{
		return _files.size();
	}

	int columnCount(const QModelIndex&) const
	{
		return 2;
	}

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override
	{
		const int row = index.row();
		Q_ASSERT(row <= _files.size());
		const auto& file = _files[row];

		if (index.column() == 0)
		{
			switch (role)
			{
				case Qt::CheckStateRole:
					return file.second ? Qt::Checked : Qt::Unchecked;
				case Qt::DisplayRole:
					return file.second ? "Yes" : "No";
			}
		}

		if (index.column() == 1)
		{
			switch (role)
			{
				case Qt::DisplayRole:
					return QDir::toNativeSeparators(file.first.absoluteFilePath());
				case Qt::DecorationRole:
					return QApplication::style()->standardIcon(file.first.isDir() ? QStyle::SP_DirIcon : QStyle::SP_FileIcon);
			}
		}

		return QVariant();
	}

	bool setData(const QModelIndex& index, const QVariant& value, int role) override
	{
		if (role == Qt::CheckStateRole && index.column() == 0)
		{
			const int row = index.row();
			Q_ASSERT(row <= _files.size());
			_files[row].second = value.toBool();
			emit dataChanged(index, index);
			return true;
		}

		return false;
	}

	bool removeRows(int row, int count, const QModelIndex& parent) override
	{
		const int lastIndex = row + count - 1;
		beginRemoveRows(parent, row, lastIndex);

		for (int index = row; index < lastIndex; ++index)
		{
			_files.removeAt(index);
		}

		endRemoveRows();
		return true;
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
						return "Delete after overwrite?";
					case 1:
						return "Path";
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

	void addPaths(QVector<QFileInfo>&& paths)
	{
		int before = _files.size();
		int after = before + paths.size() - 1;

		beginInsertRows(QModelIndex(), before, after);

		for (const QFileInfo& file : paths)
		{
			QPair<QFileInfo, bool> pair(file, true);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
			_files.emplaceBack(pair);
#else
			_files.append(pair);
#endif
		}

		endInsertRows();
	}

	const QVector<QPair<QFileInfo, bool>>& selectedPaths() const
	{
		return _files;
	}

private:
	QVector<QPair<QFileInfo, bool>> _files;
};

PathSelectTab::PathSelectTab(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PathSelectTab)
{
	ui->setupUi(this);

	ui->tableViewPaths->setModel(new PathSelectModel(this));

	connect(ui->pushButtonBack, &QPushButton::clicked, [this]()
	{
		emit backRequested();
	});

	connect(ui->pushButtonNext, &QPushButton::clicked, [this]()
	{
		emit nextRequested();
	});

	connect(
		ui->pushButtonAddFiles,
		&QPushButton::clicked,
		std::bind(&PathSelectTab::onAddPaths, this, QFileDialog::ExistingFiles));

	connect(
		ui->pushButtonAddFolder,
		&QPushButton::clicked,
		std::bind(&PathSelectTab::onAddPaths, this, QFileDialog::Directory));

	connect(ui->pushButtonRemove, &QPushButton::clicked, this, &PathSelectTab::onRemoveSelected);
}

PathSelectTab::~PathSelectTab()
{
	delete ui;
	qDebug();
}

void PathSelectTab::addPaths(QVector<QFileInfo>&& paths)
{
	auto model = reinterpret_cast<PathSelectModel*>(ui->tableViewPaths->model());
	Q_ASSERT(model);
	model->addPaths(std::move(paths));
}

const QVector<QPair<QFileInfo, bool>>& PathSelectTab::selectedPaths() const
{
	auto model = reinterpret_cast<PathSelectModel*>(ui->tableViewPaths->model());
	Q_ASSERT(model);
	return model->selectedPaths();
}

void PathSelectTab::onAddPaths(QFileDialog::FileMode mode)
{
	QFileDialog dialog(this);
	dialog.setFileMode(mode);

	if (dialog.exec() != QFileDialog::Accepted)
	{
		return;
	}

	QVector<QFileInfo> fileInfos;
	const QStringList selectedFiles = dialog.selectedFiles();

	std::transform(
		selectedFiles.cbegin(),
		selectedFiles.cend(),
		std::inserter(fileInfos, fileInfos.end()),
		[](const QString& path){ return QFileInfo(path); });

	auto model = reinterpret_cast<PathSelectModel*>(ui->tableViewPaths->model());
	Q_ASSERT(model);
	model->addPaths(std::move(fileInfos));
}

void PathSelectTab::onRemoveSelected()
{
	auto model = reinterpret_cast<PathSelectModel*>(ui->tableViewPaths->model());
	Q_ASSERT(model);

	const QModelIndexList selection = ui->tableViewPaths->selectionModel()->selectedRows();

	for (const QModelIndex& index : selection)
	{
		model->removeRow(index.row());
	}
}
