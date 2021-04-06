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
		_root(new Node(nullptr))
	{
		auto alpha = new Node(_root);
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

		alpha->_children.append(progressA1);
		alpha->_children.append(progressA2);

		auto bravo = new Node(_root);
		bravo->path = "D:\\TEST DATA\\bar.txt";
		auto progressB1 = new Node(bravo);
		auto progressB2 = new Node(bravo);

		bravo->_children.append(progressB1);
		bravo->_children.append(progressB2);

		_root->_children.append(alpha);
		_root->_children.append(bravo);
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
						return node->path;

					return QString("Pass %1").arg(index.row() + 1);
				}
				case 1:
					return QVariant::fromValue(node->secondsTaken);
				case 2:
					return QVariant::fromValue(node->secondsLeft);
				case 3:
					return QVariant::fromValue(node->bytesWritten);
				case 4:
					return QVariant::fromValue(node->bytesWritten);
				case 5:
					return QVariant::fromValue(node->bytesPerSecond);
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
	Node* _root;
};

ProgressTab::ProgressTab(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ProgressTab)
{
	ui->setupUi(this);

	auto model = new ProgressModel(this);
	ui->treeViewProgress->setModel(model);
	ui->treeViewProgress->setItemDelegateForColumn(6, new ProgressBarDelegate(this));

	connect(ui->pushButtonNext, &QPushButton::clicked, [this]
	{
		emit nextRequested(); // TODO: remove
	});
}

ProgressTab::~ProgressTab()
{
	delete ui;
	qDebug();
}

void ProgressTab::onOverwriteStarted(uint16_t passes, uint64_t bytesToWritePerPass)
{
	_bytesToProcess = bytesToWritePerPass * passes;
	_overwriteStartTime = QDateTime::currentDateTime();
	ui->progressBar->setValue(0);

	qDebug() << "Wipe started:" << passes << '/' << bytesToWritePerPass;
}

void ProgressTab::onPassStarted(const std::filesystem::path& path, uint16_t pass)
{
	_bytesProcessed[pass] = 0;

	qDebug() << "Pass started:" << toQString(path) << '/' << pass;
}

void ProgressTab::onPassProgressed(const std::filesystem::path&, uint16_t pass, uint64_t bytesWritten)
{
	_bytesProcessed[pass] = bytesWritten;
	updateTotalProgress();
}

void ProgressTab::onPassFinished(const std::filesystem::path& path, uint16_t pass)
{
	qDebug() << "Pass finished:" << toQString(path) << '/' << pass;
}

void ProgressTab::onOverwriteCompleted(uint16_t passes, uint64_t totalBytesWritten)
{
	qDebug() << "Wipe completed:" << passes << '/' << totalBytesWritten;

	ui->progressBar->setValue(100);
}

void ProgressTab::updateTotalProgress()
{
	const QDateTime now = QDateTime::currentDateTime();
	double bytesWrittenTotal = 0;

	for (const auto& [pass, bytesWritten] : _bytesProcessed)
	{
		bytesWrittenTotal += bytesWritten;
	}

	{
		const double percentage = (bytesWrittenTotal / _bytesToProcess) * 100.0f;
		ui->progressBar->setValue(static_cast<int>(percentage));
	}

	const double milliSecondsTaken = _overwriteStartTime.msecsTo(now);

	if (bytesWrittenTotal >= 0.00f && milliSecondsTaken >= 0.00f)
	{
		double bytesPerMilliSecond = bytesWrittenTotal / milliSecondsTaken;
		QString speed = SystemLocale.formattedDataSize(static_cast<qint64>(bytesPerMilliSecond * 1000.0f)).append("/s");
		ui->labelSpeed->setText(speed);

		double bytesLeft = _bytesToProcess - bytesWrittenTotal;
		double milliSecondsLeft = bytesLeft / bytesPerMilliSecond;
		QString timeLeft = SystemLocale.toString(ZeroTime.addMSecs(milliSecondsLeft), QLocale::LongFormat);
		ui->labelTime->setText(timeLeft);
	}
}
