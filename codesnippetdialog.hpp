//
// codename Morning Star
//
// Copyright (C) 2012 by Ignacio Riquelme Morelle <shadowm2006@gmail.com>
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
	void on_saveButton_clicked();
};

#endif // CODESNIPPETDIALOG_HPP
