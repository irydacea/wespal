//
// codename Morning Star
//
// Copyright (C) 2008 - 2019 by Iris Morelle <shadowm2006@gmail.com>
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

#include "wesnothrc.hpp"

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
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

	bool initial_open(const QString& initial_file);

	QStringList do_run_jobs(QMap<QString, rc_map> &jobs);

protected:
    void changeEvent(QEvent *e);
	void closeEvent(QCloseEvent *e);
	void dragEnterEvent(QDragEnterEvent *e);
	void dropEvent(QDropEvent *e);
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void wheelEvent(QWheelEvent *event);

private:
	QMap< QString, color_range > color_ranges_;
	QMap< QString, QList<QRgb> > palettes_;

	QMap< QString, color_range > user_color_ranges_;
	QMap< QString, QList<QRgb> > user_palettes_;

	/**
	 * Merges user definitions with built-ins.
	 *
	 * It is assumed that the user definitions have already been
	 * loaded when calling this method.
	 */
	void generateMergedRcDefinitions();

	/** Process definitions, updating UI elements accordingly. */
	void processRcDefinitions();

	void insertRangeListItem(const QString& id, const QString& display_name);

	Ui::MainWindow *ui;

	QString img_path_;

	QImage img_original_;
	QImage img_transview_;

	qreal zoom_;
	bool ignore_drops_;
	bool drag_use_rc_;
	bool drag_start_;
	QPoint drag_start_pos_;

	QVector<QAction*> recent_file_acts_;

	QList<qreal> zoom_factors_;

	void update_recent_files_menu();

	void update_window_title(const QString& open_filename) {
		QString display_string;

		if(open_filename.isEmpty()) {
			this->setWindowFilePath("");
			display_string = tr("Dropped file");
		} else {
			this->setWindowFilePath(open_filename);
			display_string = QFileInfo(open_filename).fileName();
		}

		this->setWindowTitle(display_string + QString().fromUtf8(" \342\200\224 ") + tr("Wesnoth RCX"));
	}

	void toggle_page1(bool newstate);
	void toggle_page2(bool newstate);

	void do_save();
	void do_close();
	void do_open(const QString& initial_file = QString());
	void do_reload();
	void do_about();

	bool confirm_existing_files(const QStringList& paths);

	QStringList do_save_color_ranges(QString& base);
	QStringList do_save_single_recolor(QString& base);

	void refresh_previews();

	QString current_pal_name(bool palette_switch_mode = false) const;
	QList<QRgb> current_pal_data(bool palette_switch_mode = false) const;

	QString supported_file_patterns() const;

	void update_zoom_buttons();

	void setPreviewBackgroundColor(const QString &colorName);

	void centerScrollArea(QScrollArea* scrollArea);

	void do_custom_preview_color_option();
	void do_custom_preview_color_icon();

private slots:
	void on_action_Reload_triggered();
	void on_actionColor_ranges_triggered();
	void on_tbZoomOut_clicked();
	void on_tbZoomIn_clicked();
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
};
