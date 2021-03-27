#include "KuuraGUI-PCH.hpp"
#include "ProgressTab.hpp"
#include "ProgressBarDelegate.hpp"
#include "ui_ProgressTab.h"

#include <QStandardItemModel>

enum class CellDisplayType : int
{
	Path = 0,
	TimeTaken,
	TimeLeft,
	BytesWritten,
	BytesLeft,
	Speed,
	Progress
};

int timeToSeconds(const QTime& t) // WTF, why QTime sucks so bad?
{
	int result = 0;

	if (t.hour() > 0)
	{
		result += t.hour() * 3600;
	}

	if (t.minute() > 0)
	{
		result += t.minute() * 60;
	}

	if (t.second() > 0)
	{
		result += t.second();
	}

	return result;
}

struct Node
{
	Node(Node* parent, const QMap<CellDisplayType, QVariant>& data) :
		m_parent(parent),
		m_data(data)
	{
	}

	~Node()
	{
		qDeleteAll(m_children);
	}

	int parentRow() const
	{
		return m_parent ?
			m_parent->m_children.indexOf(const_cast<Node*>(this)) :
			0;
	}

	Node* m_parent;
	QVector<Node*> m_children;
	QMap<CellDisplayType, QVariant> m_data;
};

class ProgressModel : public QAbstractItemModel
{
public:
	ProgressModel(QObject* parent) :
		QAbstractItemModel(parent),
		m_root(new Node(nullptr, { }))
	{
		auto alpha = new Node(m_root, { { CellDisplayType::Path, "alpha.txt" } });
		auto progressA1 = new Node(alpha,
		{
			{ CellDisplayType::TimeTaken, QTime(3, 2, 1) },
			{ CellDisplayType::TimeLeft, QTime(0, 0, 0) },
			{ CellDisplayType::BytesWritten, 444 },
			{ CellDisplayType::BytesLeft, 000 },
			{ CellDisplayType::Speed, "123 MiB/s" },
			{ CellDisplayType::Progress, 100.0f },
		});

		auto progressA2 = new Node(alpha,
		{
			{ CellDisplayType::TimeTaken, QTime(1, 2, 3) },
			{ CellDisplayType::TimeLeft, QTime(3, 2, 1) },
			{ CellDisplayType::BytesWritten, 123 },
			{ CellDisplayType::BytesLeft, 321 },
			{ CellDisplayType::Speed, "123 MiB/s" },
			{ CellDisplayType::Progress, 38.32f },
		});

		alpha->m_children.append(progressA1);
		alpha->m_children.append(progressA2);

		auto bravo = new Node(m_root, { { CellDisplayType::Path, "bravo.txt" } });
		auto progressB1 = new Node(bravo,
		{
			{ CellDisplayType::BytesLeft, 12345 },
			{ CellDisplayType::Progress, 0 },
		});
		auto progressB2 = new Node(bravo,
		{
			{ CellDisplayType::BytesLeft, 12345 },
			{ CellDisplayType::Progress, 0 },
		});

		bravo->m_children.append(progressB1);
		bravo->m_children.append(progressB2);

		m_root->m_children.append(alpha);
		m_root->m_children.append(bravo);
	}

	~ProgressModel()
	{
		delete m_root;
	}

	QModelIndex index(int row, int column, const QModelIndex& parentIndex = QModelIndex()) const override
	{
		if (!hasIndex(row, column, parentIndex))
		{
			return QModelIndex();
		}

		Node* parentNode = parentIndex.isValid() ?
			static_cast<Node*>(parentIndex.internalPointer()) :
			m_root;

		Node* childNode = parentNode->m_children.at(row);

		return childNode ?
			createIndex(row, column, childNode) :
			QModelIndex();
	}

	QModelIndex parent(const QModelIndex& childIndex) const override
	{
		if (!childIndex.isValid())
		{
			return QModelIndex();
		}

		Node* parentNode = static_cast<Node*>(childIndex.internalPointer())->m_parent;

		return parentNode != m_root ?
			createIndex(parentNode->parentRow(), 0, parentNode) :
			QModelIndex();
	}

	int rowCount(const QModelIndex& parentIndex = QModelIndex()) const override
	{
		return parentIndex.isValid() ?
			static_cast<Node*>(parentIndex.internalPointer())->m_children.size() :
			m_root->m_children.size();
	}

	int columnCount(const QModelIndex&) const override
	{
		// Progress is the last column
		return static_cast<int>(CellDisplayType::Progress) + 1;
	}

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override
	{
		if (!index.isValid())
		{
			return QVariant();
		}

		Node* node = static_cast<Node*>(index.internalPointer());
		bool topLevel = node->m_parent == m_root;
		bool step = index.column() == 0 && !topLevel;
		bool pathCell = index.column() == 0 && topLevel;
		bool progressCell = index.column() >= 1 && index.column() <= 6 && !topLevel;

		if (role == Qt::DisplayRole)
		{
			const auto cellType = static_cast<CellDisplayType>(index.column());

			if (step)
			{
				return QString("Pass %1").arg(index.row() + 1);
			}

			if (pathCell)
			{
				return node->m_data[CellDisplayType::Path];
			}

			if (progressCell)
			{
				return node->m_data[cellType];
			}


			switch (cellType)
			{
				case CellDisplayType::TimeTaken:
				case CellDisplayType::TimeLeft:
				{
					Q_ASSERT(topLevel);

					int seconds = 0;

					for (Node* childNode : node->m_children)
					{
						seconds += timeToSeconds(childNode->m_data[cellType].toTime());
					}

					QTime time(0, 0, 0);
					time = time.addSecs(seconds);

					if (time > QTime(0, 0, 0))
						return time;
				}

				case CellDisplayType::BytesWritten:
				case CellDisplayType::BytesLeft:
				{
					Q_ASSERT(topLevel);

					qulonglong bytes = 0;

					for (Node* childNode : node->m_children)
					{
						bytes += childNode->m_data[cellType].toULongLong();
					}

					if (bytes > 0)
						return bytes;
				}

				case CellDisplayType::Progress:
				{
					Q_ASSERT(topLevel);

					float sum = 0;
					float childCount = node->m_children.size();

					for (Node* childNode : node->m_children)
					{
						sum += childNode->m_data[cellType].toInt();
					}

					if (sum > 0 && childCount > 0)
						return sum / childCount;
				}
			}
		}

		return QVariant();
	}

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override
	{
		if (role != Qt::DisplayRole)
		{
			return QVariant();
		}

		if (orientation == Qt::Horizontal)
		{
			const auto cellType = static_cast<CellDisplayType>(section);

			switch (cellType)
			{
				case CellDisplayType::Path:
					return "Path";
				case CellDisplayType::TimeTaken:
					return "Time taken";
				case CellDisplayType::TimeLeft:
					return "Time left";
				case CellDisplayType::BytesWritten:
					return "Bytes written";
				case CellDisplayType::BytesLeft:
					return "Bytes left";
				case CellDisplayType::Speed:
					return "Speed";
				case CellDisplayType::Progress:
					return "Progress";
			}
		}

		return QVariant();
	}

private:
	Node* m_root;
};

ProgressTab::ProgressTab(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ProgressTab)
{
	ui->setupUi(this);

	ui->treeViewProgress->setModel(new ProgressModel(this));
	ui->treeViewProgress->setItemDelegateForColumn(
		static_cast<int>(CellDisplayType::Progress),
		new ProgressBarDelegate(this));
}

ProgressTab::~ProgressTab()
{
	delete ui;
	qDebug();
}
