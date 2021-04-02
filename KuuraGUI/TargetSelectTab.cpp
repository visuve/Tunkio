#include "KuuraGUI-PCH.hpp"
#include "TargetSelectTab.hpp"
#include "ui_TargetSelectTab.h"

TargetSelectTab::TargetSelectTab(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::TargetSelectTab)
{
	ui->setupUi(this);

	connect(ui->radioButtonPath, &QRadioButton::clicked, [this]()
	{
		_selectedTargetType = KuuraTargetType::FileOverwrite;
		emit targetTypeSelected(KuuraTargetType::FileOverwrite);
	});

	connect(ui->radioButtonDrive, &QRadioButton::clicked, [this]()
	{
		_selectedTargetType = KuuraTargetType::DriveOverwrite;
		emit targetTypeSelected(KuuraTargetType::DriveOverwrite);
	});

	connect(ui->pushButtonBack, &QPushButton::clicked, [this]()
	{
		emit backRequested();
	});

	connect(ui->pushButtonNext, &QPushButton::clicked, [this]()
	{
		emit nextRequested();
	});
}

TargetSelectTab::~TargetSelectTab()
{
	delete ui;
	qDebug();
}

KuuraTargetType TargetSelectTab::selectedTargetType() const
{
	return _selectedTargetType;
}
