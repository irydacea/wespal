/*
 * Wespal (codename Morning Star) - Wesnoth assets recoloring tool
 *
 * Copyright (C) 2011 - 2024 by Iris Morelle <iris@irydacea.me>
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

#pragma once

#include <QDialog>

#include "wesnothrc.hpp"

#include <QMap>
#include <QListWidgetItem>

namespace Ui {
    class CustomPalettes;
}

class CustomPalettes : public QDialog {
    Q_OBJECT
public:
	CustomPalettes(const QMap<QString, ColorList>& initialPalettes,
				   const QMap<QString, ColorRange>& colorRanges,
				   QWidget* parent = nullptr);

	~CustomPalettes();

	const QMap<QString, ColorList>& getPalettes() const
	{
		return palettes_;
	}

protected:
    void changeEvent(QEvent *e);

private slots:
	void on_listPals_currentRowChanged(int currentRow);

	void on_listColors_currentRowChanged(int currentRow);

	void on_tbEditColor_clicked();

	void on_listColors_itemChanged(QListWidgetItem *item);

	void on_cmdAddCol_clicked();

	void on_cmdDelCol_clicked();

	void on_cmdDelPal_clicked();

	void on_cmdAddPal_clicked();

	void on_listPals_itemChanged(QListWidgetItem *item);

	void on_cmdAddFromList_clicked();

	void on_cmdWml_clicked();

	void on_leColor_textEdited(const QString &arg1);

	void on_action_Rename_triggered();

	void on_action_Duplicate_triggered();

	void handleRcOption();

private:
    Ui::CustomPalettes *ui;
	//PaletteWidget *palw;

	QMap<QString, ColorList> palettes_;

	QMap<QString, ColorRange> ranges_;

	QMenu* createRangesMenu();
	void addRangesMenuEntry(QMenu* menu,
							const QString& id,
							const ColorRange& range,
							const QString& text);

	void addPaletteListEntry(const QString& name);
	void removePaletteListEntry(const QString& name);

	void renamePaletteListEntry(const QString& old_name,
								const QString& new_name)
	{
		removePaletteListEntry(old_name);
		addPaletteListEntry(new_name);
	}

	void populatePaletteView(const ColorList& pal);

	/**
	 * Update the palette UI starting from scratch.
	 */
	void updatePaletteUI();

	void setColorEditControlsEnabled(bool enabled);
	void setPaletteViewEnabled(bool enabled);
	void setPaletteEditControlsEnabled(bool enabled);

	void clearPaletteView();

	ColorList& getCurrentPalette();

	void updatePaletteIcon();

	QString generateNewPaletteName(QString stem = QString()) const;
};
