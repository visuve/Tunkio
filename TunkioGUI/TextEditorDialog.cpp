#include "TextEditorDialog.hpp"
#include "ui_TextEditorDialog.h"
#include <QDebug>

TextEditorDialog::TextEditorDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::TextEditorDialog)
{
	ui->setupUi(this);
}

TextEditorDialog::~TextEditorDialog()
{
	delete ui;
}

bool TextEditorDialog::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::MouseButtonDblClick)
	{
		qDebug() << "HI!";
	}

	return QObject::eventFilter(watched, event);
}
