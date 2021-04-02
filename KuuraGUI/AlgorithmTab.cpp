#include "KuuraGUI-PCH.hpp"
#include "AlgorithmTab.hpp"
#include "ui_AlgorithmTab.h"

struct OverWriteRequest
{
	bool verify;
	KuuraFillType fillType;
	QByteArray fillValue;
};

namespace Ui
{
	QVariant booleanToYesNo(bool value)
	{
		return value ? "Yes" : "No";
	}

	QString fillTypeToString(KuuraFillType type)
	{
		switch (type)
		{
			case KuuraFillType::ZeroFill:
				return "Zeroes";
			case KuuraFillType::OneFill:
				return "Ones";
			case KuuraFillType::ByteFill:
				return "Byte";
			case KuuraFillType::SequenceFill:
				return "Sequence";
			case KuuraFillType::FileFill:
				return "File";
			case KuuraFillType::RandomFill:
				return "Random";
		}

		qCritical() << int(type) << " is out of bounds";
		return "Unknown?";
	}

	QVariant fillValueToString(const OverWriteRequest& pass)
	{
		switch (pass.fillType)
		{
			case KuuraFillType::FileFill:
				return QString(pass.fillValue);
			case KuuraFillType::RandomFill:
				return "Random";
			default:
				return "0x" + pass.fillValue.toHex(':').toUpper().replace(':', " 0x");
		}
	}
}

class AlgorithmModel : public QAbstractTableModel
{
public:
	AlgorithmModel(QObject* parent) :
		QAbstractTableModel(parent)
	{
	}

	~AlgorithmModel()
	{
		qDebug();
	}

	int rowCount(const QModelIndex&) const override
	{
		return _request.size();
	}

	int columnCount(const QModelIndex&) const override
	{
		return 3;
	}

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override
	{
		if (role != Qt::DisplayRole || index.row() > _request.size())
		{
			return {};
		}

		const OverWriteRequest& requ = _request[index.row()];

		switch (index.column())
		{
			case 0:
				return Ui::booleanToYesNo(requ.verify);
			case 1:
				return Ui::fillTypeToString(requ.fillType);
			case 2:
				return Ui::fillValueToString(requ);
		}

		return {};
	}

	QVariant headerData(int section, Qt::Orientation orientation, int role) const override
	{
		if (role != Qt::DisplayRole)
		{
			return {};
		}

		if (orientation == Qt::Orientation::Horizontal)
		{
			switch (section)
			{
				case 0:
					return "Verify?";
				case 1:
					return "Fill type";
				case 2:
					return "Fill value";
			}
		}

		return section + 1;
	}

	void addOverwritePass(KuuraFillType fillType, bool verify, const QByteArray& fillValue = QByteArray())
	{
		int row = _request.size();
		beginInsertRows(QModelIndex(), row, row);

		OverWriteRequest req = {};
		req.fillType = fillType;

		switch (fillType)
		{
			case ZeroFill:
			{
				Q_ASSERT(fillValue.isEmpty());
				req.fillValue = QByteArray("\x00", 1);
				break;
			}
			case OneFill:
			{
				Q_ASSERT(fillValue.isEmpty());
				req.fillValue = QByteArray("\xFF", 1);
				break;
			}
			case ByteFill:
			{
				Q_ASSERT(fillValue.size() == 1);
				req.fillValue = fillValue;
				break;
			}
			case RandomFill:
			{
				Q_ASSERT(fillValue.isEmpty());
				break;
			}
			default:
			{
				Q_ASSERT(!fillValue.isEmpty());
				req.fillValue = fillValue;
				break;
			}
		}

		req.verify = verify;
		_request.append(req);
		endInsertRows();
	}

	void clearRequests()
	{
		if (!_request.isEmpty())
		{
			beginResetModel();
			_request.clear();
			endResetModel();
		}
	}

private:
	QVector<OverWriteRequest> _request;
};

AlgorithmTab::AlgorithmTab(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::AlgorithmTab)
{
	ui->setupUi(this);

	ui->tableViewWipePasses->setModel(new AlgorithmModel(this));
	ui->groupBoxAddPass->setEnabled(false);

	connect(ui->comboBoxPresets,
		static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
		this,
		&AlgorithmTab::presetChanged);

	emit presetChanged(0);

	connect(ui->pushButtonBack, &QPushButton::clicked, [this]()
	{
		emit backRequested();
	});

	connect(ui->pushButtonNext, &QPushButton::clicked, [this]()
	{
		emit nextRequested();
	});

	connect(ui->radioButtonCustomize, &QRadioButton::clicked, [this]()
	{
		ui->tableViewWipePasses->setEditTriggers(QAbstractItemView::NoEditTriggers);
		ui->groupBoxAddPass->setEnabled(true);
		ui->comboBoxPresets->setEnabled(false);
	});

	connect(ui->radioButtonPresets, &QRadioButton::clicked, [this]()
	{
		ui->tableViewWipePasses->setEditTriggers(QAbstractItemView::AllEditTriggers);
		ui->groupBoxAddPass->setEnabled(false);
		ui->comboBoxPresets->setEnabled(true);
	});
}

AlgorithmTab::~AlgorithmTab()
{
	delete ui;
	qDebug();
}

QVector<QPair<KuuraFillType, QByteArray> > AlgorithmTab::selectedAlgorithms()
{
	return {}; // TODO!
}

void AlgorithmTab::presetChanged(int index)
{
	auto model = reinterpret_cast<AlgorithmModel*>(ui->tableViewWipePasses->model());

	model->clearRequests();

	switch (index)
	{
		case 0:
			model->addOverwritePass(KuuraFillType::ZeroFill, true);
			break;
		case 1:
			model->addOverwritePass(KuuraFillType::RandomFill, true);
			break;
		case 2:
			model->addOverwritePass(KuuraFillType::OneFill, false);
			model->addOverwritePass(KuuraFillType::ZeroFill, false);
			model->addOverwritePass(KuuraFillType::RandomFill, true);
			break;
		case 3:
			model->addOverwritePass(KuuraFillType::ZeroFill, false);
			model->addOverwritePass(KuuraFillType::OneFill, false);
			model->addOverwritePass(KuuraFillType::RandomFill, false);
			break;
		case 4:
			QMessageBox::critical(
				this,
				"Kuura - Not implemented yet!",
				"This feature is not yet implemented");
			break;
		case 5:
			model->addOverwritePass(KuuraFillType::ByteFill, false, "\x55");
			model->addOverwritePass(KuuraFillType::ByteFill, false, "\xAA");
			model->addOverwritePass(KuuraFillType::RandomFill, false);
			break;

		case 6:
			model->addOverwritePass(KuuraFillType::OneFill, false);
			model->addOverwritePass(KuuraFillType::ZeroFill, false);
			model->addOverwritePass(KuuraFillType::RandomFill, false);
			model->addOverwritePass(KuuraFillType::RandomFill, false);
			model->addOverwritePass(KuuraFillType::RandomFill, false);
			model->addOverwritePass(KuuraFillType::RandomFill, false);
			model->addOverwritePass(KuuraFillType::RandomFill, false);
			break;
		case 7:
		{
			thread_local std::random_device device;
			thread_local std::mt19937_64 engine(device());

			std::array<std::pair<KuuraFillType, QByteArray>, 35> gutmannsRecipe =
			{
				std::make_pair(KuuraFillType::RandomFill, QByteArray()),
				{ KuuraFillType::RandomFill, {} },
				{ KuuraFillType::RandomFill, {} },
				{ KuuraFillType::RandomFill, {} },

				{ KuuraFillType::ByteFill, "\x55" },
				{ KuuraFillType::ByteFill, "\xAA" },
				{ KuuraFillType::SequenceFill, "\x92\x49\x24" },
				{ KuuraFillType::SequenceFill, "\x49\x24\x92" },
				{ KuuraFillType::SequenceFill, "\x24\x92\x49" },
				{ KuuraFillType::ZeroFill, {} },
				{ KuuraFillType::ByteFill, "\x11" },
				{ KuuraFillType::ByteFill, "\x22" },
				{ KuuraFillType::ByteFill, "\x33" },
				{ KuuraFillType::ByteFill, "\x44" },
				{ KuuraFillType::ByteFill, "\x55" },
				{ KuuraFillType::ByteFill, "\x66" },
				{ KuuraFillType::ByteFill, "\x77" },
				{ KuuraFillType::ByteFill, "\x88" },
				{ KuuraFillType::ByteFill, "\x99" },
				{ KuuraFillType::ByteFill, "\xAA" },
				{ KuuraFillType::ByteFill, "\xBB" },
				{ KuuraFillType::ByteFill, "\xCC" },
				{ KuuraFillType::ByteFill, "\xDD" },
				{ KuuraFillType::ByteFill, "\xEE" },
				{ KuuraFillType::OneFill, {} },
				{ KuuraFillType::SequenceFill, "\x92\x49\x24" },
				{ KuuraFillType::SequenceFill, "\x49\x24\x92" },
				{ KuuraFillType::SequenceFill, "\x24\x92\x49" },
				{ KuuraFillType::SequenceFill, "\x6D\xB6\xDB" },
				{ KuuraFillType::SequenceFill, "\xB6\xDB\x6D" },
				{ KuuraFillType::SequenceFill, "\xDB\x6D\xB6" },

				{ KuuraFillType::RandomFill, {} },
				{ KuuraFillType::RandomFill, {} },
				{ KuuraFillType::RandomFill, {} },
				{ KuuraFillType::RandomFill, {} },
			};

			do
			{
				model->clearRequests();

				std::shuffle(gutmannsRecipe.begin() + 4, gutmannsRecipe.end() - 4, engine);

				for (const auto& [type, value] : gutmannsRecipe)
				{
					model->addOverwritePass(type, false, value);
				}

			}
			while (QMessageBox::question(
				this,
				"Kuura - Shuffle",
				"Gutmann's method requires random order of some of the passes.\n"
				"Click retry to shuffle, OK to accept.",
				QMessageBox::Retry | QMessageBox::Ok) == QMessageBox::Retry);

			break;
		}
		case 8:
			model->addOverwritePass(KuuraFillType::ByteFill, false, { 1, '\x00' });
			model->addOverwritePass(KuuraFillType::ByteFill, false, "\xFF");
			model->addOverwritePass(KuuraFillType::RandomFill, true);
			break;
		case 9:
			model->addOverwritePass(KuuraFillType::ByteFill, false, "\xFF");
			model->addOverwritePass(KuuraFillType::ByteFill, false, { 1, '\x00' });
			model->addOverwritePass(KuuraFillType::RandomFill, true);
			break;
	}
}
