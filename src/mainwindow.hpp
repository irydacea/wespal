/*
 * Wespal (codename Morning Star) - Wesnoth assets recoloring tool
 *
 * Copyright (C) 2008 - 2024 by Iris Morelle <iris@irydacea.me>
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

#include "appconfig.hpp"

#include <QAbstractButton>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QFileInfo>
#include <QMainWindow>
#include <QScrollArea>
#include <QStringList>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

	/**
	 * Opens the specified file, or prompts the user to select one.
	 */
	void openFile(const QString& fileName = {});

	QStringList doRunJobs(const QMap<QString, ColorMap>& jobs);

protected:
    void changeEvent(QEvent *e);
	void closeEvent(QCloseEvent *e);
	void dragEnterEvent(QDragEnterEvent *e);
	void dropEvent(QDropEvent *e);
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);

private:
	using ViewMode = MosConfig::ImageViewMode;

	enum RcMode
	{
		RcColorRange,
		RcPaletteSwap,
	};

	enum ZoomDirection
	{
		ZoomIn,
		ZoomOut,
	};

	// These can only be values that look good with NN scaling
	static constexpr std::array zoomFactors_ = {
		0.5,
		1.0,
		2.0,
		4.0,
		8.0
	};

	Ui::MainWindow *ui;

	QMap<QString, ColorRange> colorRanges_;
	QMap<QString, ColorList> palettes_;

	QMap<QString, ColorRange> userColorRanges_;
	QMap<QString, ColorList> userPalettes_;

	QString imagePath_;

	QImage originalImage_;
	QImage transformedImage_;

	ViewMode viewMode_;
	RcMode   rcMode_;

	qreal zoom_;

	bool ignoreDrops_;
	bool dragUseRecolored_;
	bool dragStart_;
	QPoint dragStartPos_;

	QList<QAction*> recentFileActions_;
	QList<QAction*> zoomActions_;
	QList<QAction*> viewModeActions_;

	QString supportedImageFileFormats_;

	/**
	 * Merges user definitions with built-ins.
	 *
	 * It is assumed that the user definitions have already been
	 * loaded when calling this method.
	 */
	void generateMergedRcDefinitions();

	/** Process definitions, updating UI elements accordingly. */
	void processRcDefinitions();

	void insertRangeListItem(const QString& id, const QString& display_name, const QColor& color);

	void updateRecentFilesMenu();

	void updateWindowTitle(bool hasImage, const QString& filename = {});

	void doSaveFile();
	void doCloseFile();
	void doReloadFile();
	void doAboutDialog();

	void setViewMode(ViewMode newViewMode);
	void setRcMode(RcMode rcMode);
	void enableWorkArea(bool enable);

	bool confirmFileOverwrite(const QStringList& paths);

	QStringList doSaveColorRanges(QString& base);
	QStringList doSaveSingleRecolor(QString& base);

	void refreshPreviews();

	QString currentPaletteName(bool paletteSwitchMode = false) const;
	ColorList currentPalette(bool paletteSwitchMode = false) const;

	void adjustZoom(ZoomDirection direction);

	void setPreviewBackgroundColor(const QString &colorName);

	void centerScrollArea(QScrollArea* scrollArea);

	void doCustomPreviewBgSelect();
	void updateCustomPreviewBgIcon();

private slots:
	void on_action_Reload_triggered();
	void on_actionColor_ranges_triggered();
	void on_listRanges_currentRowChanged(int currentRow);
	void on_cbxNewPal_currentIndexChanged(int index);
	void on_cbxKeyPal_currentIndexChanged(int index);
	void on_action_Save_triggered();
	void on_action_Quit_triggered();
	void on_action_Open_triggered();
	void on_buttonBox_clicked(QAbstractButton* button);
	void on_actionAbout_Morning_Star_triggered();
	void on_radPal_clicked();
	void on_radRc_clicked();
	void handleRecent();
	void on_zoomSlider_valueChanged(int value);
	void on_action_Palettes_triggered();

	void handlePreviewBgOption(bool checked);
	void on_cmdOpen_clicked();
	void on_action_ClearMru_triggered();
	void on_action_Close_triggered();
	void on_cbxViewMode_currentIndexChanged(int index);
	void on_viewSlider_valueChanged(int value);
	void on_actionZoomIn_triggered();
	void on_actionZoomOut_triggered();
};
