#include "colorlistinputdialog.hpp"
#include "ui_colorlistinputdialog.h"

ColorListInputDialog::ColorListInputDialog(QWidget *parent) :
    QDialog(parent),
	ui(new Ui::ColorListInputDialog),
	listString_()
{
	ui->setupUi(this);
}

ColorListInputDialog::~ColorListInputDialog()
{
	delete ui;
}

void ColorListInputDialog::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void ColorListInputDialog::on_buttonBox_accepted()
{
	listString_ = ui->teColors->toPlainText();
}

QList<QRgb> ColorListInputDialog::getColorList() const
{
	QList<QRgb> ret;

	if(!listString_.isEmpty()) {
		const QStringList& colorNames =
			listString_.split(",", QString::SkipEmptyParts);

		foreach(const QString& originalColorName, colorNames) {
			if(originalColorName.isEmpty())
				continue;

			QString colorName = originalColorName;

			if(colorName.at(0) != '#') {
				colorName.prepend('#');
			}

			QColor color(colorName);

			if(color.isValid()) {
				ret.push_back(color.rgb());
			}
		}
	}

	return ret;
}
