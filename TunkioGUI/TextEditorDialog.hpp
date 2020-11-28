#pragma once

namespace Ui
{
	class TextEditorDialog;
}

class TextEditorDialog : public QDialog
{
	Q_OBJECT

public:
	explicit TextEditorDialog(QLineEdit* parent = nullptr);
	~TextEditorDialog();

	bool eventFilter(QObject* watched, QEvent* event);

	QString text();
	void setText(const QString&);

private:
	Ui::TextEditorDialog* ui = nullptr;
};

