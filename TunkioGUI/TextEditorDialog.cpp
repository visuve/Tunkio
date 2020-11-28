#include "TunkioGUI-PCH.hpp"
#include "TextEditorDialog.hpp"
#include "ui_TextEditorDialog.h"

TextEditorDialog::TextEditorDialog(QLineEdit* parent) :
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
	switch (event->type())
	{
		case QEvent::MouseButtonDblClick:
			setText(dynamic_cast<QLineEdit*>(parent())->text());
			this->show();
			return true;
	}

	return QObject::eventFilter(watched, event);
}

QString TextEditorDialog::text()
{
	return ui->plainTextEdit->toPlainText();
}

void TextEditorDialog::setText(const QString& text)
{
	ui->plainTextEdit->clear();
	ui->plainTextEdit->insertPlainText(text); // Moves the cursor in the end
}
