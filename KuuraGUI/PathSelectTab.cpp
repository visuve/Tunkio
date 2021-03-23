#include "KuuraGUI-PCH.hpp"
#include "PathSelectTab.hpp"
#include "ui_PathSelectTab.h"

class PathModel : public QAbstractListModel
{
public:
	PathModel(QObject* parent) :
		QAbstractListModel(parent)
	{
	}

	~PathModel()
	{
		qDebug();
	}

	int rowCount(const QModelIndex&) const override
	{
		return m_files.size();
	}

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override
	{
		if (role != Qt::DisplayRole)
		{
			return {};
		}

		if (index.row() > m_files.size())
		{
			return {};
		}

		const QFileInfo& file = m_files[index.row()];

		return QDir::toNativeSeparators(file.absoluteFilePath());
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

private:
	QVector<QFileInfo> m_files;
};

PathSelectTab::PathSelectTab(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PathSelectTab)
{
	ui->setupUi(this);

	ui->listViewPaths->setModel(new PathModel(this));

	connect(ui->pushButtonAddFiles, &QPushButton::clicked, std::bind(&PathSelectTab::onAddPaths, this, false));
	connect(ui->pushButtonAddFolder, &QPushButton::clicked, std::bind(&PathSelectTab::onAddPaths, this, true));
}

PathSelectTab::~PathSelectTab()
{
	delete ui;
	qDebug();
}

void PathSelectTab::onAddPaths(bool isDirectory)
{
	QFileDialog dialog(this);
	dialog.setFileMode(isDirectory ? QFileDialog::Directory : QFileDialog::ExistingFiles);

	if (dialog.exec() != QFileDialog::Accepted)
	{
		return;
	}

	const QStringList selectedFiles = dialog.selectedFiles();

	auto model = reinterpret_cast<PathModel*>(ui->listViewPaths->model());

	Q_ASSERT(model);

	model->addPaths(selectedFiles);
}
