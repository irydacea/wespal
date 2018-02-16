//
// codename Morning Star
//
// Copyright (C) 2011 - 2018 by Iris Morelle <shadowm2006@gmail.com>
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

#ifndef CUSTOMRANGES_HPP
#define CUSTOMRANGES_HPP

#include <QDialog>

#include "wesnothrc.hpp"

#include <QAbstractButton>
#include <QListWidgetItem>

namespace Ui {
    class CustomRanges;
}

class CustomRanges : public QDialog {
    Q_OBJECT
public:
	CustomRanges(const QMap<QString, color_range>& initialRanges, QWidget *parent);
    ~CustomRanges();

	const QMap<QString, color_range>& ranges() const {
		return ranges_;
	}

protected:
	void changeEvent(QEvent *e);

private:
    Ui::CustomRanges *ui;

	QMap<QString, color_range> ranges_;

	color_range& currentRange();

	void updateColorButton(QAbstractButton* button, const QColor& color);
	void updateRangeEditControls();

	QString generateNewRangeName(QString stem = QString()) const;

	void addRangeListEntry(const QString& name);

private slots:
	void on_cmdAdd_clicked();
	void on_cmdDelete_clicked();

	void on_leMin_textChanged(QString);
	void on_leMax_textChanged(QString);
	void on_leAvg_textChanged(QString);

	void on_tbMin_clicked();
	void on_tbMax_clicked();
	void on_tbAvg_clicked();
	void on_listRanges_currentRowChanged(int);
	void on_listRanges_itemChanged(QListWidgetItem *item);
	void on_cmdWml_clicked();

	void on_action_Rename_triggered();
	void on_action_Duplicate_triggered();
};

#endif // CUSTOMRANGES_HPP
