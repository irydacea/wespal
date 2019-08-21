//
// codename Morning Star
//
// Copyright (C) 2012 - 2019 by Iris Morelle <shadowm2006@gmail.com>
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

#pragma once

#include <QDialog>

namespace Ui {
class ColorListInputDialog;
}

class ColorListInputDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit ColorListInputDialog(QWidget *parent = nullptr);
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
