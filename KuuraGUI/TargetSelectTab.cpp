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
		m_selectedTargetType = KuuraTargetType::FileWipe;
	});

	connect(ui->radioButtonDrive, &QRadioButton::clicked, [this]()
	{
		m_selectedTargetType = KuuraTargetType::DriveWipe;
	});

	connect(ui->pushButtonBack, &QPushButton::clicked, [this]()
	{
		emit backRequested();
	});

	connect(ui->pushButtonNext, &QPushButton::clicked, [this]()
	{
		emit targetTypeSelected(m_selectedTargetType);
	});
}

TargetSelectTab::~TargetSelectTab()
{
	delete ui;
	qDebug();
}
