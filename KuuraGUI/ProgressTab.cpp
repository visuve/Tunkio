#include "KuuraGUI-PCH.hpp"
#include "ProgressTab.hpp"
#include "ProgressBarDelegate.hpp"
#include "ui_ProgressTab.h"

#include <QStandardItemModel>

struct Node
{
	Node(Node* parent) :
		m_parent(parent)
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

	QString path;
	uint64_t secondsTaken = 0;
	uint64_t secondsLeft = 0;
	uint64_t bytesWritten = 0;
	uint64_t bytesToWrite = 0;
	uint64_t bytesPerSecond = 0;
	float progressPercent = 0;
};

class ProgressModel : public QAbstractItemModel
{
public:
	ProgressModel(QObject* parent) :
		QAbstractItemModel(parent),
		m_root(new Node(nullptr))
	{
		auto alpha = new Node(m_root);
		alpha->path = "D:\\TEST DATA\\foo.txt";

		auto progressA1 = new Node(alpha);
		progressA1->secondsTaken = 444;
		progressA1->secondsLeft = 0;
		progressA1->bytesWritten = 444;
		progressA1->bytesToWrite = 0;
		progressA1->bytesPerSecond = 1;
		progressA1->progressPercent = 100.0f;

		auto progressA2 = new Node(alpha);
		progressA2->secondsTaken = 321;
		progressA2->secondsLeft = 123;
		progressA2->bytesWritten = 321;
		progressA2->bytesToWrite = 123;
		progressA2->bytesPerSecond = 1;
		 progressA2->progressPercent = 38.32f;

		alpha->secondsTaken = progressA1->secondsTaken + progressA2->secondsTaken;
		alpha->secondsLeft = progressA1->secondsLeft + progressA2->secondsLeft;
		alpha->bytesWritten = progressA1->bytesWritten + progressA2->bytesWritten;
		alpha->bytesToWrite = progressA1->secondsTaken + progressA2->secondsTaken;
		alpha->bytesPerSecond = progressA1->bytesPerSecond + progressA2->bytesPerSecond / 2;		 
		alpha->progressPercent = (progressA1->progressPercent + progressA2->progressPercent) / 2;

		alpha->m_children.append(progressA1);
		alpha->m_children.append(progressA2);

		auto bravo = new Node(m_root);
		bravo->path = "D:\\TEST DATA\\bar.txt";
		auto progressB1 = new Node(bravo);
		auto progressB2 = new Node(bravo);

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
		return 7;
	}

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override
	{
		if (!index.isValid())
		{
			return QVariant();
		}

		Node* node = static_cast<Node*>(index.internalPointer());
		bool topLevel = node->m_parent == m_root;

		if (role == Qt::DisplayRole)
		{
			switch (index.column())
			{
				case 0:
				{
					if (topLevel)
						return node->path;

					return QString("Pass %1").arg(index.row() + 1);
				}
				case 1:
					return node->secondsTaken;
				case 2:
					return node->secondsLeft;
				case 3:
					return node->bytesWritten;
				case 4:
					return node->bytesWritten;
				case 5:
					return node->bytesPerSecond;
				case 6:
					return node->progressPercent;
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
			switch (section)
			{
				case 0:
					return "Path";
				case 1:
					return "Time taken";
				case 2:
					return "Time left";
				case 3:
					return "Bytes written";
				case 4:
					return "Bytes left";
				case 5:
					return "Speed";
				case 6:
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

	auto model = new ProgressModel(this);
	ui->treeViewProgress->setModel(model);
	ui->treeViewProgress->setItemDelegateForColumn(6, new ProgressBarDelegate(this));

	// TODO: remove
	connect(ui->pushButtonNext, &QPushButton::clicked, [this]
	{
		emit overwriteFinished();
	});
}

ProgressTab::~ProgressTab()
{
	delete ui;
	qDebug();
}
