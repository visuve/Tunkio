#include "KuuraGUI-PCH.hpp"
#include "ProgressTab.hpp"
#include "ui_ProgressTab.h"

class TreeItem
{
public:
	explicit TreeItem(const QVector<QVariant>& data, TreeItem* parent) :
		m_itemData(data),
		m_parentItem(parent)
	{
	}

	~TreeItem()
	{
		qDeleteAll(m_childItems);
		qDebug() << m_itemData;
	}

	void appendChild(TreeItem* item)
	{
		m_childItems.append(item);
	}

	TreeItem* childItem(int row)
	{
		if (row < 0 || row >= m_childItems.size())
		{
			return nullptr;
		}

		return m_childItems.at(row);
	}

	int childCount() const
	{
		return m_childItems.count();
	}

	int columnCount() const
	{
		return m_itemData.count();
	}

	QVariant data(int column) const
	{
		if (column < 0 || column >= m_itemData.size())
		{
			return QVariant();
		}

		return m_itemData.at(column);
	}

	TreeItem* parentItem()
	{
		return m_parentItem;
	}

	int row() const
	{
		if (!m_parentItem)
		{
			return 0;
		}

		return m_parentItem->m_childItems.indexOf(this);
	}

private:
	QVector<TreeItem*> m_childItems;
	QVector<QVariant> m_itemData;
	TreeItem* m_parentItem;
};

class ProgressModel : public QAbstractItemModel
{
public:
	ProgressModel(QObject* parent = nullptr) :
		QAbstractItemModel(parent)
	{
		rootItem = new TreeItem({"Path", "Progress"}, nullptr);

		auto alpha = new TreeItem({"alpha.txt"}, rootItem);
		alpha->appendChild(new TreeItem({1, 2, 3}, alpha));
		alpha->appendChild(new TreeItem({4, 5, 6}, alpha));
		alpha->appendChild(new TreeItem({7, 8, 9}, alpha));

		auto bravo = new TreeItem({"bravo.txt"}, rootItem);
		bravo->appendChild(new TreeItem({9, 8, 7}, bravo));
		bravo->appendChild(new TreeItem({6, 5, 4}, bravo));
		bravo->appendChild(new TreeItem({3, 2, 1}, bravo));

		rootItem->appendChild(alpha);
		rootItem->appendChild(bravo);
	}

	~ProgressModel()
	{
		delete rootItem;
	}

	int columnCount(const QModelIndex& parentIndex) const override
	{
		TreeItem* parentItem = parentIndex.isValid() ?
			static_cast<TreeItem*>(parentIndex.internalPointer()) :
			rootItem;

		return parentItem->columnCount();
	}

	int rowCount(const QModelIndex& parentIndex) const override
	{
		if (parentIndex.column() > 0)
		{
			return 0;
		}

		TreeItem* parentItem = parentIndex.isValid() ?
			static_cast<TreeItem*>(parentIndex.internalPointer()) :
			rootItem;

		return parentItem->childCount();
	}

	QVariant data(const QModelIndex& index, int role) const override
	{
		if (!index.isValid() || role != Qt::DisplayRole)
		{
			return QVariant();
		}

		TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
		return item->data(index.column());
	}

	QVariant headerData(int section, Qt::Orientation orientation, int role) const override
	{
		if (role != Qt::DisplayRole)
		{
			return QVariant();
		}

		if (orientation == Qt::Vertical)
		{
			return section + 1;
		}

		return rootItem->data(section);
	}

	QModelIndex index(int row, int column, const QModelIndex& parentIndex) const override
	{
		if (!hasIndex(row, column, parentIndex))
		{
			return QModelIndex();
		}

		TreeItem* parentItem = parentIndex.isValid() ?
			static_cast<TreeItem*>(parentIndex.internalPointer()) :
			rootItem;

		TreeItem* childItem = parentItem->childItem(row);

		return childItem ?
			createIndex(row, column, childItem) :
			QModelIndex();
	}

	QModelIndex parent(const QModelIndex& childIndex) const override
	{
		if (!childIndex.isValid())
		{
			return QModelIndex();
		}

		TreeItem* childItem = static_cast<TreeItem*>(childIndex.internalPointer());
		TreeItem* parentItem = childItem->parentItem();

		return parentItem != rootItem ?
			createIndex(parentItem->row(), 0, parentItem) :
			QModelIndex();
	}

private:
	TreeItem* rootItem;
};

ProgressTab::ProgressTab(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ProgressTab)
{
	ui->setupUi(this);

	ui->treeViewProgress->setModel(new ProgressModel(this));
}

ProgressTab::~ProgressTab()
{
	delete ui;
	qDebug();
}
