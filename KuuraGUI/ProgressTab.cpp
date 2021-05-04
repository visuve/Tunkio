#include "KuuraGUI-PCH.hpp"
#include "ProgressTab.hpp"
#include "ProgressBarDelegate.hpp"
#include "ui_ProgressTab.h"

#include <QStandardItemModel>

using MilliSeconds = std::chrono::duration<float, std::ratio<1, 1000>>;

namespace
{
	QString toQString(const std::filesystem::path& path)
	{
		return QString::fromStdString(path.string());
	}

	const QTime ZeroTime(0, 0, 0, 0);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	const QLocale SystemLocale = QLocale::system();
#else
	QLocale SystemLocale = QLocale::system();
#endif
}

struct Node
{
	Node(Node* parent) :
		_parent(parent)
	{
	}

	~Node()
	{
		qDeleteAll(_children);
	}

	int parentRow() const
	{
		return _parent ?
			_parent->_children.indexOf(const_cast<Node*>(this)) :
			0;
	}

	Node* _parent;
	QVector<Node*> _children;

	std::filesystem::path path;
	float bytesWritten = 0.0f;
	float bytesLeft = 0.0f;
	MilliSeconds millisecondsTaken;
	MilliSeconds millisecondsLeft;
	float bytesPerMilliSecond = 0.0f;
	float progressPercent = 0.0f;

	std::chrono::steady_clock::time_point startTime;
};

class ProgressModel : public QAbstractItemModel
{
public:
	ProgressModel(QObject* parent) :
		QAbstractItemModel(parent),
		_root(new Node(nullptr))
	{
	}

	~ProgressModel()
	{
		delete _root;
	}

	QModelIndex index(int row, int column, const QModelIndex& parentIndex = QModelIndex()) const override
	{
		if (!hasIndex(row, column, parentIndex))
		{
			return QModelIndex();
		}

		Node* parentNode = parentIndex.isValid() ?
			static_cast<Node*>(parentIndex.internalPointer()) :
			_root;

		Node* childNode = parentNode->_children.at(row);

		return childNode ?
			createIndex(row, column, childNode) :
			QModelIndex();
	}

	QModelIndex parent(const QModelIndex& childIndex) const override
	{
		Q_ASSERT(childIndex.isValid());

		if (!childIndex.isValid())
		{
			return QModelIndex();
		}

		Node* parentNode = static_cast<Node*>(childIndex.internalPointer())->_parent;

		return parentNode != _root ?
			createIndex(parentNode->parentRow(), 0, parentNode) :
			QModelIndex();
	}

	int rowCount(const QModelIndex& parentIndex = QModelIndex()) const override
	{
		return parentIndex.isValid() ?
			static_cast<Node*>(parentIndex.internalPointer())->_children.size() :
			_root->_children.size();
	}

	int columnCount(const QModelIndex&) const override
	{
		return 7;
	}

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override
	{
		Q_ASSERT(index.isValid());

		if (!index.isValid())
		{
			return QVariant();
		}

		Node* node = static_cast<Node*>(index.internalPointer());
		bool topLevel = node->_parent == _root;

		if (role == Qt::DisplayRole)
		{
			switch (index.column())
			{
				case 0:
				{
					if (topLevel)
					{
						return toQString(node->path);
					}

					return QString("Pass %1").arg(index.row() + 1);
				}
				case 1:
					if (topLevel)
					{
						break;
					}

					return SystemLocale.formattedDataSize(node->bytesWritten);
				case 2:
					if (topLevel)
					{
						break;
					}

					return SystemLocale.formattedDataSize(node->bytesLeft);
				case 3:
					if (topLevel)
					{
						break;
					}

					return SystemLocale.toString(ZeroTime.addMSecs(node->millisecondsTaken.count()), QLocale::LongFormat);
				case 4:
					if (topLevel)
					{
						break;
					}

					return SystemLocale.toString(ZeroTime.addMSecs(node->millisecondsLeft.count()), QLocale::LongFormat);
				case 5:
					if (topLevel)
					{
						break;
					}

					return SystemLocale.formattedDataSize(node->bytesPerMilliSecond * 1000.0f).append("/s");
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

	void addNode(const std::filesystem::path& path)
	{
		auto node = new Node(_root);
		node->path = path;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
		_root->_children.emplaceBack(node);
#else
		_root->_children.append(node);
#endif
	}

	void setPassCount(uint16_t passes)
	{
		for (Node* iter : _root->_children)
		{
			for (uint16_t i = 0; i < passes; ++i)
			{
				auto node = new Node(iter);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
				iter->_children.emplaceBack(node);
#else
				iter->_children.append(node);
#endif
			}
		}

		emit dataChanged(index(0, 0), index(passes, 1), { Qt::DisplayRole });
	}

	std::pair<int, Node*> findTopLevelNode(const std::filesystem::path& path)
	{
		Node* topLevelNode = nullptr;
		int parentRow = 0;

		for (Node* iter : _root->_children)
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
		
	std::pair<int, Node*> findProgressNode(const std::filesystem::path& path, uint16_t pass)
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

	void beginTarget(const std::filesystem::path& path, uint64_t bytesToWrite)
	{
		auto node = findTopLevelNode(path);

		if (!node.second)
		{
			return;
		}

		for (Node* progressNode : node.second->_children)
		{
			progressNode->bytesLeft = bytesToWrite;
		}

		const auto now = std::chrono::steady_clock::now();
		node.second->startTime = now;
		node.second->_children.first()->startTime = now;
	}

	void updateNode(const std::filesystem::path& path, uint16_t pass, uint64_t bytesWritten)
	{
		auto iter = findProgressNode(path, pass);
		int parentRow = iter.first;
		Node* node = iter.second;

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
			node->millisecondsTaken =  std::chrono::duration_cast<MilliSeconds>(now - node->startTime);
			node->bytesPerMilliSecond = node->millisecondsTaken.count() ? node->bytesWritten / node->millisecondsTaken.count() : 0;
			node->millisecondsLeft = MilliSeconds(node->bytesLeft / node->bytesPerMilliSecond);
		}

		int row = pass - 1;
		Q_ASSERT(row >= 0);
		emit dataChanged(index(parentRow, 1), index(row, 6), { Qt::DisplayRole });
	}

	float bytesWrittenTotal()
	{
		float result = 0;

		for (Node* topLevel : _root->_children)
		{
			for (Node* iter : topLevel->_children)
			{
				result += iter->bytesWritten;
			}
		}

		return result;
	}

private:
	Node* _root;
};

ProgressTab::ProgressTab(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ProgressTab)
{
	ui->setupUi(this);

	ui->treeViewProgress->setModel(new ProgressModel(this));
	ui->treeViewProgress->setItemDelegateForColumn(6, new ProgressBarDelegate(this));

	connect(ui->pushButtonNext, &QPushButton::clicked, [this]
	{
		emit nextRequested();
	});
}

ProgressTab::~ProgressTab()
{
	delete ui;
	qDebug();
}

void ProgressTab::addTarget(const std::filesystem::path& path)
{
	auto model = reinterpret_cast<ProgressModel*>(ui->treeViewProgress->model());
	Q_ASSERT(model);
	model->addNode(path);
}

void ProgressTab::onOverwriteStarted(uint16_t passes, uint64_t bytesToWritePerPass)
{
	_bytesToProcess = float(bytesToWritePerPass) * float(passes);
	_overwriteStartTime = std::chrono::steady_clock::now();
	ui->progressBar->setValue(0);
	ui->pushButtonNext->setEnabled(false);

	auto model = reinterpret_cast<ProgressModel*>(ui->treeViewProgress->model());
	Q_ASSERT(model);
	model->setPassCount(passes);

	qDebug() << "Wipe started:" << passes << '/' << bytesToWritePerPass;
}

void ProgressTab::onTargetStarted(const std::filesystem::path& path, uint64_t bytesToWrite)
{
	auto model = reinterpret_cast<ProgressModel*>(ui->treeViewProgress->model());
	Q_ASSERT(model);
	model->beginTarget(path, bytesToWrite);
}

void ProgressTab::onPassStarted(const std::filesystem::path& path, uint16_t pass)
{

	qDebug() << "Pass started:" << toQString(path) << '/' << pass;
}

void ProgressTab::onPassProgressed(const std::filesystem::path& path, uint16_t pass, uint64_t bytesWritten)
{
	updateTotalProgress();

	auto model = reinterpret_cast<ProgressModel*>(ui->treeViewProgress->model());
	Q_ASSERT(model);
	model->updateNode(path, pass, bytesWritten);
}

void ProgressTab::onPassCompleted(const std::filesystem::path& path, uint16_t pass)
{
	qDebug() << "Pass completed:" << toQString(path) << '/' << pass;
}

void ProgressTab::onTargetCompleted(const std::filesystem::path& path, uint64_t bytesWritten)
{
	qDebug() << "Target completed:" << toQString(path) << '/' << bytesWritten;
}

void ProgressTab::onOverwriteCompleted(uint16_t passes, uint64_t totalBytesWritten)
{
	qDebug() << "Wipe completed:" << passes << '/' << totalBytesWritten;

	updateTotalProgress();
	ui->progressBar->setValue(100);
	ui->pushButtonNext->setEnabled(true);
}

void ProgressTab::updateTotalProgress()
{
	auto model = reinterpret_cast<ProgressModel*>(ui->treeViewProgress->model());
	Q_ASSERT(model);
	const float bytesWrittenTotal = model->bytesWrittenTotal();

	const std::chrono::steady_clock::time_point time = std::chrono::steady_clock::now();
	const float milliSecondsTaken = std::chrono::duration_cast<MilliSeconds>(time - _overwriteStartTime).count();

	if (bytesWrittenTotal <= 0.00f || milliSecondsTaken <= 0.00f)
	{
		return;
	}

	const float bytesPerMilliSecond = bytesWrittenTotal / milliSecondsTaken;

	{
		const QString timeTaken = SystemLocale.toString(ZeroTime.addMSecs(milliSecondsTaken), QLocale::LongFormat);
		ui->labelTimeTaken->setText(timeTaken);
	}
	{
		const float bytesPerSecond = bytesPerMilliSecond * 1000.0f;
		const QString speed = SystemLocale.formattedDataSize(bytesPerSecond).append("/s");
		ui->labelSpeed->setText(speed);
	}
	{
		const float bytesLeft = _bytesToProcess - bytesWrittenTotal;
		const float milliSecondsLeft = bytesLeft / bytesPerMilliSecond;
		const QString timeLeft = SystemLocale.toString(ZeroTime.addMSecs(milliSecondsLeft), QLocale::LongFormat);
		ui->labelTimeLeft->setText(timeLeft);
	}
	{
		const float percentage = (bytesWrittenTotal / _bytesToProcess) * 100.0f;
		ui->progressBar->setValue(static_cast<int>(percentage));
	}
}
