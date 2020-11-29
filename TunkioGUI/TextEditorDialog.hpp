#pragma once

namespace Ui
{
	class TextEditorDialog;
}

class TextEditorDialog : public QDialog
{
	Q_OBJECT

public:
	explicit TextEditorDialog(QWidget* parent = nullptr);
	virtual ~TextEditorDialog();

	QString text();
	void setText(const QString&);

private:
	Ui::TextEditorDialog* ui = nullptr;
};

