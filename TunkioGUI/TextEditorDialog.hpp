#pragma once

namespace Ui
{
	class TextEditorDialog;
}

class TextEditorDialog : public QDialog
{
	Q_OBJECT

public:
	explicit TextEditorDialog(QWidget *parent = nullptr);
	~TextEditorDialog();

	bool eventFilter(QObject* watched, QEvent* event);

private:
	Ui::TextEditorDialog *ui;
};

