#include "ResultsTab.hpp"
#include "ui_ResultsTab.h"

ResultsTab::ResultsTab(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ResultsTab)
{
	ui->setupUi(this);
}

ResultsTab::~ResultsTab()
{
	delete ui;
}
