#include "KuuraGUI-PCH.hpp"
#include "ProgressModel.hpp"

using MilliSeconds = std::chrono::duration<float, std::ratio<1, 1000>>;

class ProgressNode
{
public:
	ProgressNode(ProgressNode* parent) :
		_parent(parent)
	{
	}

	~ProgressNode()
	{
		qDeleteAll(_children);
	}

	int row() const
	{
		return _parent ?
			_parent->_children.indexOf(const_cast<ProgressNode*>(this)) :
			0;
	}

	const ProgressNode* _parent;
	QVector<ProgressNode*> _children;

	std::filesystem::path path;
	float bytesWritten = 0.0f;
	float bytesLeft = 0.0f;
	MilliSeconds millisecondsTaken;
	MilliSeconds millisecondsLeft;
	float bytesPerMilliSecond = 0.0f;
	float progressPercent = 0.0f;

	std::chrono::steady_clock::time_point startTime;
};

ProgressModel::ProgressModel(QObject* parent) :
	QAbstractItemModel(parent),
	_root(new ProgressNode(nullptr))
{
	/*auto alpha = new ProgressNode(_root);
	alpha->path = "D:\\TEST DATA\\foo.txt";

	auto progressA1 = new ProgressNode(alpha);
	progressA1->millisecondsTaken = MilliSeconds(1000);
	progressA1->millisecondsLeft = MilliSeconds(0);
	progressA1->bytesWritten = 10000;
	progressA1->bytesLeft = 0;
	progressA1->bytesPerMilliSecond = 10;
	progressA1->progressPercent = 100.0f;

	alpha->_children.append(progressA1);

	_root->_children.append(alpha);*/
}

ProgressModel::~ProgressModel()
{
	delete _root;
}

QModelIndex ProgressModel::index(int row, int column, const QModelIndex& parentIndex) const
{
	if (!hasIndex(row, column, parentIndex))
	{
		return QModelIndex();
	}

	ProgressNode* parentNode = parentIndex.isValid() ?
		static_cast<ProgressNode*>(parentIndex.internalPointer()) :
		_root;

	ProgressNode* childNode = parentNode->_children.at(row);

	return childNode ?
		createIndex(row, column, childNode) :
		QModelIndex();
}

QModelIndex ProgressModel::parent(const QModelIndex& childIndex) const
{
	Q_ASSERT(childIndex.isValid());

	if (!childIndex.isValid())
	{
		return QModelIndex();
	}

	const ProgressNode* parentNode = static_cast<ProgressNode*>(childIndex.internalPointer())->_parent;

	return parentNode != _root ?
		createIndex(parentNode->row(), 0, parentNode) :
		QModelIndex();
}

int ProgressModel::rowCount(const QModelIndex& parentIndex) const
{
	return parentIndex.isValid() ?
		static_cast<ProgressNode*>(parentIndex.internalPointer())->_children.size() :
		_root->_children.size();
}

int ProgressModel::columnCount(const QModelIndex&) const
{
	return 7;
}

QVariant ProgressModel::data(const QModelIndex& index, int role) const
{
	Q_ASSERT(index.isValid());

	if (!index.isValid())
	{
		return QVariant();
	}

	ProgressNode* node = static_cast<ProgressNode*>(index.internalPointer());
	bool topLevel = node->_parent == _root;

	if (role == Qt::DisplayRole)
	{
		switch (index.column())
		{
			case 0:
			{
				if (topLevel)
				{
					return Ui::toQString(node->path);
				}

				return QString("Pass %1").arg(index.row() + 1);
			}
			case 1:
				if (topLevel)
				{
					break;
				}

				return Ui::bytesToString(node->bytesWritten);
			case 2:
				if (topLevel)
				{
					break;
				}

				return Ui::bytesToString(node->bytesLeft);
			case 3:
				if (topLevel)
				{
					break;
				}

				return Ui::millisecondsToString(node->millisecondsTaken);
			case 4:
				if (topLevel)
				{
					break;
				}

				return Ui::millisecondsToString(node->millisecondsLeft);
			case 5:
				if (topLevel)
				{
					break;
				}

				return Ui::bytesToString(node->bytesPerMilliSecond * 1000.0f).append("/s");
			case 6:
				if (topLevel)
				{
					break;
				}

				return std::clamp(node->progressPercent, 0.00f, 100.00f);
		}
	}

	return QVariant();
}

QVariant ProgressModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
	{
		return QVariant();
	}

	if (orientation == Qt::Horizontal)
	{
		switch (section)
		{
			case 0:
				return "Path";
			case 1:
				return "Bytes written";
			case 2:
				return "Bytes left";
			case 3:
				return "Time taken";
			case 4:
				return "Time left";
			case 5:
				return "Speed";
			case 6:
				return "Progress";
		}
	}

	return QVariant();
}

void ProgressModel::addNode(const std::filesystem::path& path)
{
	auto node = new ProgressNode(_root);
	node->path = path;
	_root->_children.emplaceBack(node);
}

void ProgressModel::setPassCount(uint16_t passes)
{
	for (ProgressNode* iter : _root->_children)
	{
		for (uint16_t i = 0; i < passes; ++i)
		{
			auto node = new ProgressNode(iter);
			iter->_children.emplaceBack(node);
		}
	}

	emit dataChanged(index(0, 0), index(passes, 1), { Qt::DisplayRole });
}

std::pair<int, ProgressNode*> ProgressModel::findTopLevelNode(const std::filesystem::path& path)
{
	ProgressNode* topLevelNode = nullptr;
	int parentRow = 0;

	for (ProgressNode* iter : _root->_children)
	{
		++parentRow;

		if (iter->path != path)
		{
			continue;
		}

		topLevelNode = iter;
		break;
	}

	if (!topLevelNode)
	{
		return { -1, nullptr };
	}

	return { parentRow, topLevelNode };
}

std::pair<int, ProgressNode*> ProgressModel::findProgressNode(const std::filesystem::path& path, uint16_t pass)
{
	auto node = findTopLevelNode(path);

	if (node.first < 0 || !node.second)
	{
		return { -1, nullptr };
	}

	int subRow = pass - 1;
	Q_ASSERT(subRow >= 0);

	return { node.first, node.second->_children[subRow] };
}

void ProgressModel::beginTarget(const std::filesystem::path& path, uint64_t bytesToWrite)
{
	auto node = findTopLevelNode(path);

	if (!node.second)
	{
		return;
	}

	for (ProgressNode* progressNode : node.second->_children)
	{
		progressNode->bytesLeft = bytesToWrite;
	}

	const auto now = std::chrono::steady_clock::now();
	node.second->startTime = now;
	node.second->_children.first()->startTime = now;
}

void ProgressModel::updateNode(const std::filesystem::path& path, uint16_t pass, uint64_t bytesWritten)
{
	auto iter = findProgressNode(path, pass);
	int parentRow = iter.first;
	ProgressNode* node = iter.second;

	if (parentRow < 0 || !node)
	{
		return;
	}

	node->bytesWritten = bytesWritten;

	if (node->bytesWritten > 0 && node->bytesLeft > 0)
	{
		const auto now = std::chrono::steady_clock::now();
		node->bytesLeft -= bytesWritten;
		node->progressPercent = node->bytesWritten / node->bytesLeft * 100.0f;
		node->millisecondsTaken = std::chrono::duration_cast<MilliSeconds>(now - node->startTime);
		node->bytesPerMilliSecond = node->millisecondsTaken.count() ? node->bytesWritten / node->millisecondsTaken.count() : 0;
		node->millisecondsLeft = MilliSeconds(node->bytesLeft / node->bytesPerMilliSecond);
	}

	int row = pass - 1;
	Q_ASSERT(row >= 0);
	emit dataChanged(index(parentRow, 1), index(row, 6), { Qt::DisplayRole });
}

float ProgressModel::bytesWrittenTotal() const
{
	float result = 0;

	for (ProgressNode* topLevel : _root->_children)
	{
		for (ProgressNode* iter : topLevel->_children)
		{
			result += iter->bytesWritten;
		}
	}

	return result;
}
