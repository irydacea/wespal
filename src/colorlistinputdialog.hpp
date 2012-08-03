#ifndef COLORLISTINPUTDIALOG_HPP
#define COLORLISTINPUTDIALOG_HPP

#include <QDialog>

namespace Ui {
class ColorListInputDialog;
}

class ColorListInputDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit ColorListInputDialog(QWidget *parent = 0);
	~ColorListInputDialog();

	QList<QRgb> getColorList() const;
	
protected:
	void changeEvent(QEvent *e);
	
private slots:
	void on_buttonBox_accepted();

private:
	Ui::ColorListInputDialog *ui;
	QString listString_;
};

#endif // COLORLISTINPUTDIALOG_HPP
