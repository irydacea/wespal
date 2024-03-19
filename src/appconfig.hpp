//
// codename Morning Star
//
// Copyright (C) 2010 - 2019 by Iris Morelle <shadowm2006@gmail.com>
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

#include <QList>
#include <QSize>
#include <QStringList>

enum PreviewBackgroundColor {
	SystemColor,
	White,
};

void mos_config_load(QMap<QString, ColorRange>& ranges,
					 QMap<QString, ColorList>& palettes);

void mos_config_save(const QMap<QString, ColorRange>& ranges,
					 const QMap<QString, ColorList>& palettes);

QStringList mos_recent_files();
void mos_add_recent_file(const QString& filepath);
void mos_remove_recent_file(const QString& filepath);

unsigned mos_max_recent_files();

QString mos_get_preview_background_color_name();
void mos_set_preview_background_color_name(const QString& colorName);

QSize mos_get_main_window_size();
void mos_set_main_window_size(const QSize& size);
