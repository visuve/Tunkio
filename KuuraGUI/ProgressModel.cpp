#include "KuuraGUI-PCH.hpp"
#include "ProgressModel.hpp"

using MilliSeconds = std::chrono::duration<float, std::ratio<1, 1000>>;

class ProgressNode
{
public:
	explicit ProgressNode(ProgressNode* parent, const QVector<QVariant>& data) :
		_parent(parent),
		_data(data)
	{
	}

	~ProgressNode()
	{
		qDeleteAll(_children);
	}

	ProgressNode* addChild(const QVector<QVariant>& data)
	{
		return _children.emplaceBack(new ProgressNode(this, data));
	}

	ProgressNode* childAt(int row)
	{
		if (row < 0 || row >= _children.size())
		{
			return nullptr;
		}

		return _children.at(row);
	}

	QVector<ProgressNode*> findChildren(const std::function<bool(ProgressNode*)>& lambda) const
	{
		QVector<ProgressNode*> results;

		for (ProgressNode* child : _children)
		{
			if (lambda(child))
			{
				results.emplaceBack(child);
			}
		}

		return results;
	}

	int childCount() const
	{
		return _children.size();
	}

	int columnCount() const
	{
		return _data.size();
	}

	QVariant data(int column) const
	{
		if (column < 0 || column >= _data.size())
		{
			return QVariant();
		}

		return _data.at(column);
	}

	QVariantList data() const
	{
		return _data;
	}

	QVariant& operator[] (int index)
	{
		return _data[index];
	}

	int row() const
	{
		return _parent ? _parent->_children.indexOf(this) : 0;
	}

	ProgressNode* parent()
	{
		return _parent;
	}

	const QVector<ProgressNode*>& children() const
	{
		return _children;
	}

private:
	ProgressNode* _parent;
	QVector<ProgressNode*> _children;
	QVariantList _data;
};

inline ProgressNode* indexToNode(const QModelIndex& index)
{
	return static_cast<ProgressNode*>(index.internalPointer());
}

ProgressModel::ProgressModel(const QVariantList& headerData, QObject* parent) :
	QAbstractItemModel(parent),
	_root(new ProgressNode(nullptr, headerData))
{
	// _root->addChild({ "/dev/sdc" })->addChild({ "Pass 1", "7.454", "14.907GB", "24m 01s", "23m 57s", "5.682MB/s", 50.000f });
}

ProgressModel::~ProgressModel()
{
	delete _root;
}

QVariant ProgressModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
	{
		return _root->data(section);
	}

	return QVariant();
}

QModelIndex ProgressModel::index(int row, int column, const QModelIndex& parentIndex) const
{
	if (!hasIndex(row, column, parentIndex))
	{
		return QModelIndex();
	}

	ProgressNode* parent = parentIndex.isValid() ? indexToNode(parentIndex) : _root;
	ProgressNode* child = parent->childAt(row);

	return child ? createIndex(row, column, child) : QModelIndex();
}

QModelIndex ProgressModel::parent(const QModelIndex& childIndex) const
{
	if (!childIndex.isValid())
	{
		return QModelIndex();
	}

	ProgressNode* child = indexToNode(childIndex);
	ProgressNode* parent = child->parent();

	return parent == _root ?
		QModelIndex() :
		createIndex(parent->row(), 0, parent);
}

int ProgressModel::rowCount(const QModelIndex& parentIndex) const
{
	if (parentIndex.column() > 0)
	{
		return 0;
	}

	return parentIndex.isValid() ? indexToNode(parentIndex)->childCount() : _root->childCount();
}

int ProgressModel::columnCount(const QModelIndex&) const
{
	return _root->columnCount();
}

QVariant ProgressModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
	{
		return QVariant();
	}

	if (role != Qt::DisplayRole)
	{
		return QVariant();
	}

	return indexToNode(index)->data(index.column());
}

void ProgressModel::addTarget(const QVariant& path)
{
	_root->addChild({ path });
}

void ProgressModel::addPass(const QVariantList& pass)
{
	for (ProgressNode* node : _root->children())
	{
		node->addChild(pass);
	}
}

bool ProgressModel::setData(const QString& path, int row, int column, const QVariant& value)
{
	QVector<ProgressNode*> nodes = _root->findChildren([&](ProgressNode* node)->bool
	{
		if (row == -1)
		{
			return node->data(0) == path;
		}

		return node->row() == row && node->data(0) == path;
	});

	for (ProgressNode* node : nodes)
	{
		(*node)[column] = value;
	}

	return !nodes.isEmpty();
}

/*QVariantList* ProgressModel::data(const QString& path, int row, int column)
{
	for (ProgressNode* node : _root->children())
	{
		if (node->row() == row && node->data(0) == path)
		{
			return &node->data();
		}
	}

	return nullptr;
}*/

