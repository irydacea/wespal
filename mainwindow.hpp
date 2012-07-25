//
// codename Morning Star
//
// Copyright (C) 2008, 2009, 2010, 2011 by Ignacio R. Morelle <shadowm@wesnoth.org>
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

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "wesnothrc.hpp"
#include "specs.hpp"

#include <QAbstractButton>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMainWindow>
#include <QScrollBar>
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
	void dragEnterEvent(QDragEnterEvent *e);
	void dropEvent(QDropEvent *e);
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);

	QList< range_spec > color_ranges_;
	QList< pal_spec   > palettes_;

	QList< range_spec > user_ranges_;
	QList< pal_spec   > user_palettes_;

	void initialize_specs();
	void update_ui_from_specs();	

private:
    Ui::MainWindow *ui;

	QString img_path_;

	QImage img_original_;
	QImage img_transview_;

	float zoom_;
	bool ignore_drops_;
	bool drag_use_rc_;
	bool drag_start_;
	QPoint drag_start_pos_;

	void toggle_page1(bool newstate);
	void toggle_page2(bool newstate);

	void do_save();
	void do_close();
	void do_open(const QString& initial_file = QString());
	void do_reload();
	void do_about();

	bool confirm_existing_files(QStringList& paths);

	QStringList do_save_color_ranges(QString& base);
	QStringList do_save_single_recolor(QString& base);

	void refresh_previews();

	QString current_pal_name(bool palette_switch_mode = false) const;
	QList<QRgb> const *current_pal_data(bool palette_switch_mode = false) const;

	QString supported_file_patterns() const;

	void update_zoom_buttons();

private slots:
	void on_action_Reload_triggered();
 void on_actionColor_ranges_triggered();
 void on_tbZoomOut_clicked();
 void on_tbZoomIn_clicked();
 void on_cbxZoomFactor_currentIndexChanged(int index);
	void on_listRanges_itemSelectionChanged();
	void on_cbxNewPal_currentIndexChanged(int index);
	void on_cbxKeyPal_currentIndexChanged(int index);
	void on_action_Save_triggered();
	void on_action_Quit_triggered();
	void on_action_Open_triggered();
	void on_buttonBox_clicked(QAbstractButton* button);
	void on_actionAbout_Morning_Star_triggered();
	void on_radPal_clicked();
	void on_radRc_clicked();
};

#endif // MAINWINDOW_HPP
