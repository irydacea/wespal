/*
 * Wespal (codename Morning Star) - Wesnoth assets recoloring tool
 *
 * Copyright (C) 2012 - 2025 by Iris Morelle <iris@irydacea.me>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "colorlistinputdialog.hpp"
#include "ui_colorlistinputdialog.h"

ColorListInputDialog::ColorListInputDialog(QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::ColorListInputDialog)
	, listString_()
{
	ui->setupUi(this);

	setWindowFlags(Qt::Sheet);

	const auto& monoFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
	ui->teColors->setFont(monoFont);

	ui->teColors->setFocus();
}

ColorListInputDialog::~ColorListInputDialog()
{
	delete ui;
}

void ColorListInputDialog::changeEvent(QEvent* event)
{
	QDialog::changeEvent(event);

	switch (event->type()) {
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

ColorList ColorListInputDialog::getColorList() const
{
	ColorList ret;

	if (!listString_.isEmpty()) {
		const auto& colorNames = listString_.split(",", Qt::SkipEmptyParts);

		for (const auto& originalColorName : colorNames) {
			if (originalColorName.isEmpty()) {
				continue;
			}

			QString colorName = originalColorName;

			if (colorName.at(0) != '#') {
				colorName.prepend('#');
			}

			QColor color(colorName);

			if (color.isValid()) {
				ret.push_back(color.rgb());
			}
		}
	}

	return ret;
}
