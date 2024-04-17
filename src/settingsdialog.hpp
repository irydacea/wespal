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

#include "wesnothrc.hpp"

#include <QDialog>

class QAbstractButton;
class QLineEdit;
class QListWidget;
class QListWidgetItem;

namespace Ui {
class SettingsDialog;
}

/**
 * Main Wespal user settings dialog class
 */
class SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	template<typename ArtifactT>
	using ArtifactMap = QMap<QString, ArtifactT>;
	using ColorListMap = ArtifactMap<ColorList>;
	using ColorRangeMap = ArtifactMap<ColorRange>;

	explicit SettingsDialog(const QList<qreal>& zoomValues,
							const QImage& referenceImage,
							QWidget* parent = nullptr);

	~SettingsDialog();

	bool rememberLayout() const;
	void setRememberLayout(bool value) const;

	bool rememberViewMode() const;
	void setRememberViewMode(bool value) const;

	qreal defaultZoom() const;
	void setDefaultZoom(qreal value);

protected:
	virtual void changeEvent(QEvent* event) override;

private slots:
	void onDialogAccepted();

private:
	Ui::SettingsDialog *ui;

	QList<qreal> zoomValues_;
	ColorListMap palettes_;
	ColorRangeMap ranges_;

	QImage referenceImage_;

	QMenu* paletteRecolorMenu_;
	QMap<QString, QAction*> paletteRecolorActions_;

	//
	// Utility methods
	//

	void showCodeSnippet(const QString& title, const QString& text);

	QString itemData(const QListWidgetItem* item);

	void updateColorButton(QAbstractButton* button, const QColor& color);

	/**
	 * Generates an artifact name.
	 *
	 * Names returned by this method are of the form "[nameStem] #123", where
	 * 123 is a sequential unique number starting from 1. The @a artifactMap
	 * is scanned for potentially clashing names in order to find a suffix
	 * that results in a unique, non-existing name.
	 */
	template<typename MapT>
	QString generateArtifactName(const QString& nameStem,
								 const MapT& artifactMap) const
	{
		QString name;
		unsigned suffix = 1;

		do {
			name = tr("%1 #%2").arg(nameStem).arg(suffix++);
		} while (artifactMap.contains(name));

		return name;
	}

	/**
	 * Generates a unique color range name.
	 */
	QString generateColorRangeName(QString stem = {}) const
	{
		if (stem.isEmpty())
			stem = tr("New Color Range");

		return generateArtifactName(stem, ranges_);
	}

	/**
	 * Generates a unique color palette name.
	 */
	QString generatePaletteName(QString stem = {}) const
	{
		if (stem.isEmpty())
			stem = tr("New Palette");

		return generateArtifactName(stem, palettes_);
	}

	// Generic helpers for color artifact pages

	using ColorReceiverFunction = std::function<void(const QColor&)>;

	void setupSingleColorEditSlots(QLineEdit* lineEdit,
								   QAbstractButton* button,
								   ColorReceiverFunction func);

	/**
	 * Retrieves the artifact collection object for the specified type.
	 */
	template<typename ArtifactT>
	ArtifactMap<ArtifactT>& artifactCollection();

	/**
	 * Retrieves the QListWidget for the specified artifact collection.
	 */
	template<typename ArtifactT>
	QListWidget* artifactSelector();

	/**
	 * Retrieves the currently-selected artifact of the specifed type.
	 */
	template<typename ArtifactT>
	ArtifactT& currentArtifact();

	template<typename ArtifactT>
	void artifactItemRenameInteractive(QListWidgetItem* listItem);

	//
	// General page
	//

	void initGeneralPage();

private:
	//
	// Color ranges page
	//

	void initColorRangesPage();

	void updateRangeEditControls();

	void addRangeListEntry(const QString& name);

	void updateColorRangeIcon(QListWidgetItem* listItem = nullptr);

private slots:
	void onColorRangeRowChanged(int row);
	void onColorRangeItemChanged(QListWidgetItem* item);

	void onColorRangeAdd();
	void onColorRangeDelete();

	void onColorRangeRename();
	void onColorRangeDuplicate();

	void onColorRangeFromList();
	void onColorRangeToWml();

private:
	//
	// Color palettes page
	//

	void initPalettesPage();

	void addPaletteRecolorMenuEntry(const QString& id,
									const ColorRange& colorRange,
									const QString& label);
	void deletePaletteRecolorMenuEntry(const QString& name);

	void addPaletteListEntry(const QString& name);
	void removePaletteListEntry(const QString& name);
	void renamePaletteListEntry(const QString& oldName,
								const QString& newName);

	void populatePaletteView(const ColorList& palette);
	void clearPaletteView();
	void setPaletteViewEnabled(bool enabled);
	void setPaletteEditControlsEnabled(bool enabled);
	void setPaletteColorEditControlsEnabled(bool enabled);

	void updatePaletteIcon(QListWidgetItem* listItem = nullptr);

	void appendToCurrentPalette(const ColorList& colors);

private slots:
	void onPaletteRowChanged(int row);
	void onPaletteItemChanged(QListWidgetItem* item);

	void onPaletteAdd();
	void onPaletteDelete();

	void onPaletteRename();
	void onPaletteDuplicate();
	void onPaletteRecolor();

	void onPaletteFromList();
	void onPaletteFromImage();
	void onPaletteToWml();
	void onPaletteToGpl();

	void onColorRowChanged(int row);
	void onColorItemChanged(QListWidgetItem* item);

	void onColorAdd();
	void onColorDelete();

};
