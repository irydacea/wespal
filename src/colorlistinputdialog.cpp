//
// codename Morning Star
//
// Copyright (C) 2012 - 2013 by Ignacio Riquelme Morelle <shadowm2006@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#include "colorlistinputdialog.hpp"
#include "ui_colorlistinputdialog.h"

ColorListInputDialog::ColorListInputDialog(QWidget *parent) :
    QDialog(parent),
	ui(new Ui::ColorListInputDialog),
	listString_()
{
	ui->setupUi(this);

	QFont font("Monospace");
	font.setStyleHint(QFont::TypeWriter);

	ui->teColors->setFont(font);
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
