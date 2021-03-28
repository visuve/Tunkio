#include "KuuraGUI-PCH.hpp"
#include "PathSelectTab.hpp"
#include "ui_PathSelectTab.h"

class PathSelectModel : public QAbstractListModel
{
public:
	PathSelectModel(QObject* parent) :
		QAbstractListModel(parent)
	{
	}

	~PathSelectModel()
	{
		qDebug();
	}

	int rowCount(const QModelIndex&) const override
	{
		return m_files.size();
	}

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override
	{
		if (index.row() > m_files.size())
		{
			return QVariant();
		}

		if (role == Qt::DisplayRole)
		{
			const QFileInfo& file = m_files[index.row()];
			return QDir::toNativeSeparators(file.absoluteFilePath());
		}

		if (role == Qt::DecorationRole)
		{
			const QFileInfo& file = m_files[index.row()];
			return QApplication::style()->standardIcon(file.isDir() ? QStyle::SP_DirIcon : QStyle::SP_FileIcon);
		}

		return QVariant();
	}

	void addPaths(const QStringList& files)
	{
		beginInsertRows(QModelIndex(), m_files.size(), m_files.size() + files.size());

		for (const QString& file : files)
		{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
			m_files.emplaceBack(file);
#else
			m_files.append(file);
#endif
		}

		endInsertRows();
	}

	QStringList selectedPaths() const
	{
		QStringList selected;

		for (const QFileInfo& file : m_files)
		{
			selected << QDir::toNativeSeparators(file.absoluteFilePath());
		}

		return selected;
	}

private:
	QVector<QFileInfo> m_files;
};

PathSelectTab::PathSelectTab(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PathSelectTab)
{
	ui->setupUi(this);

	ui->listViewPaths->setModel(new PathSelectModel(this));

	connect(ui->pushButtonBack, &QPushButton::clicked, [this]()
	{
		emit backRequested();
	});

	connect(
		ui->pushButtonAddFiles,
		&QPushButton::clicked,
		std::bind(&PathSelectTab::onAddPaths, this, QFileDialog::ExistingFiles));

	connect(
		ui->pushButtonAddFolder,
		&QPushButton::clicked,
		std::bind(&PathSelectTab::onAddPaths, this, QFileDialog::Directory));

	connect(ui->pushButtonNext, &QPushButton::clicked, [this]()
	{
		emit nextRequested();
	});
}

PathSelectTab::~PathSelectTab()
{
	delete ui;
	qDebug();
}

void PathSelectTab::addPaths(const QStringList& paths)
{
	auto model = reinterpret_cast<PathSelectModel*>(ui->listViewPaths->model());
	Q_ASSERT(model);
	model->addPaths(paths);
}

QStringList PathSelectTab::selectedPaths() const
{
	auto model = reinterpret_cast<PathSelectModel*>(ui->listViewPaths->model());
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

	const QStringList selectedFiles = dialog.selectedFiles();

	auto model = reinterpret_cast<PathSelectModel*>(ui->listViewPaths->model());
	Q_ASSERT(model);
	model->addPaths(selectedFiles);
}
