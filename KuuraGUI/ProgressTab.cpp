#include "KuuraGUI-PCH.hpp"
#include "ProgressTab.hpp"
#include "ProgressBarDelegate.hpp"
#include "ui_ProgressTab.h"

#include <QStandardItemModel>

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
	uint64_t bytesWritten = 0;
	uint64_t bytesLeft = 0;
	int secondsTaken = 0;
	int secondsLeft = 0;
	float bytesPerSecond = 0;
	float progressPercent = 0;

	QTime startTime;
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
					return SystemLocale.formattedDataSize(node->bytesWritten);
				case 2:
					return SystemLocale.formattedDataSize(node->bytesLeft);
				case 3:
					return SystemLocale.toString(ZeroTime.addSecs(node->secondsTaken), QLocale::LongFormat);
				case 4:
					return SystemLocale.toString(ZeroTime.addSecs(node->secondsLeft), QLocale::LongFormat);
				case 5:
					return SystemLocale.formattedDataSize(node->bytesPerSecond).append("/s");
				case 6:
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

	std::pair<int, Node*> findProgressNode(const std::filesystem::path& path, uint16_t pass)
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

		int subRow = pass - 1;
		Q_ASSERT(subRow >= 0);

		return { parentRow, topLevelNode->_children[subRow] };
	}

	void beginPass(const std::filesystem::path& path, uint16_t pass, uint64_t bytesToWrite)
	{
		auto node = findProgressNode(path, pass);

		if (!node.second)
		{
			return;
		}

		node.second->startTime = QTime::currentTime();
		node.second->bytesLeft = bytesToWrite;
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
			node->bytesLeft -= bytesWritten;
			node->progressPercent = float(node->bytesWritten) / float(node->bytesLeft) * 100.0f;
			node->secondsTaken = node->startTime.secsTo(QTime::currentTime());
			node->bytesPerSecond = node->secondsTaken ? node->bytesWritten / node->secondsTaken : 0;
			node->secondsLeft = node->bytesLeft / node->bytesPerSecond;
		}

		int row = pass - 1;
		Q_ASSERT(row >= 0);
		emit dataChanged(index(parentRow, 1), index(row, 1), { Qt::DisplayRole });
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

void ProgressTab::setPassCount(uint16_t passes)
{
	auto model = reinterpret_cast<ProgressModel*>(ui->treeViewProgress->model());
	Q_ASSERT(model);
	model->setPassCount(passes);
}

void ProgressTab::onOverwriteStarted(uint16_t passes, uint64_t bytesToWritePerPass)
{
	_bytesToProcess = bytesToWritePerPass * passes;
	_overwriteStartTime = QDateTime::currentDateTime();
	ui->progressBar->setValue(0);
	ui->pushButtonNext->setEnabled(false);

	qDebug() << "Wipe started:" << passes << '/' << bytesToWritePerPass;

	setPassCount(passes);
}

void ProgressTab::onPassStarted(const std::filesystem::path& path, uint16_t pass)
{
	_bytesProcessed[pass] = 0;
	qDebug() << "Pass started:" << toQString(path) << '/' << pass;
	auto model = reinterpret_cast<ProgressModel*>(ui->treeViewProgress->model());
	Q_ASSERT(model);
	model->beginPass(path, pass, 6666);

}

void ProgressTab::onPassProgressed(const std::filesystem::path& path, uint16_t pass, uint64_t bytesWritten)
{
	_bytesProcessed[pass] = bytesWritten;
	updateTotalProgress();

	auto model = reinterpret_cast<ProgressModel*>(ui->treeViewProgress->model());
	Q_ASSERT(model);
	model->updateNode(path, pass, bytesWritten);
}

void ProgressTab::onPassFinished(const std::filesystem::path& path, uint16_t pass)
{
	qDebug() << "Pass finished:" << toQString(path) << '/' << pass;
}

void ProgressTab::onOverwriteCompleted(uint16_t passes, uint64_t totalBytesWritten)
{
	qDebug() << "Wipe completed:" << passes << '/' << totalBytesWritten;

	ui->progressBar->setValue(100);
	ui->pushButtonNext->setEnabled(true);
}

void ProgressTab::updateTotalProgress()
{
	const QDateTime now = QDateTime::currentDateTime();
	float bytesWrittenTotal = 0;

	for (const auto& [pass, bytesWritten] : _bytesProcessed)
	{
		bytesWrittenTotal += bytesWritten;
	}

	{
		const float percentage = (bytesWrittenTotal / _bytesToProcess) * 100.0f;
		ui->progressBar->setValue(static_cast<int>(percentage));
	}

	const float milliSecondsTaken = _overwriteStartTime.msecsTo(now);

	if (bytesWrittenTotal >= 0.00f && milliSecondsTaken >= 0.00f)
	{
		const float bytesPerMilliSecond = bytesWrittenTotal / milliSecondsTaken;
		const qint64 bytesPerSecond = static_cast<qint64>(bytesPerMilliSecond * 1000.0f);
		const QString speed = SystemLocale.formattedDataSize(bytesPerSecond).append("/s");
		ui->labelSpeed->setText(speed);

		const float bytesLeft = _bytesToProcess - bytesWrittenTotal;
		const float milliSecondsLeft = bytesLeft / bytesPerMilliSecond;
		const QString timeLeft = SystemLocale.toString(ZeroTime.addMSecs(milliSecondsLeft), QLocale::LongFormat);
		ui->labelTime->setText(timeLeft);
	}
}
