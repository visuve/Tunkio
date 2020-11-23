#include "TunkioGUI-PCH.hpp"
#include "TextEditorDialog.hpp"
#include "ui_TextEditorDialog.h"

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
