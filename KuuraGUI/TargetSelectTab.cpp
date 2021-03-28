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
		emit targetTypeSelected(KuuraTargetType::FileWipe);
	});

	connect(ui->radioButtonDrive, &QRadioButton::clicked, [this]()
	{
		m_selectedTargetType = KuuraTargetType::DriveWipe;
		emit targetTypeSelected(KuuraTargetType::DriveWipe);
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
	return m_selectedTargetType;
}
