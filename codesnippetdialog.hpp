#ifndef CODESNIPPETDIALOG_HPP
#define CODESNIPPETDIALOG_HPP

#include <QDialog>

namespace Ui {
class CodeSnippetDialog;
}

class CodeSnippetDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit CodeSnippetDialog(const QString& contents, QWidget *parent = 0);
	~CodeSnippetDialog();
	
protected:
	void changeEvent(QEvent *e);
	
private:
	Ui::CodeSnippetDialog *ui;

private slots:
	void on_copyButton_clicked();
};

#endif // CODESNIPPETDIALOG_HPP
