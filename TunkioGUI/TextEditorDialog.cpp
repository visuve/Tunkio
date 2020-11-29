#include "TunkioGUI-PCH.hpp"
#include "TextEditorDialog.hpp"
#include "ui_TextEditorDialog.h"

TextEditorDialog::TextEditorDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::TextEditorDialog)
{
	ui->setupUi(this);
}

TextEditorDialog::~TextEditorDialog()
{
	delete ui;
}

QString TextEditorDialog::text()
{
	return ui->plainTextEdit->toPlainText();
}

void TextEditorDialog::setText(const QString& text)
{
	ui->plainTextEdit->clear();
	ui->plainTextEdit->insertPlainText(text); // Moves the cursor to the end
}
