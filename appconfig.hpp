//
// codename Morning Star
//
// Copyright (C) 2010 - 2012 by Ignacio Riquelme Morelle <shadowm2006@gmail.com>
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

#ifndef APPCONFIG_HPP
#define APPCONFIG_HPP

#include "specs.hpp"

#include <QList>
#include <QStringList>

void mos_config_load(QMap< QString, color_range >& ranges, QMap< QString, QList<QRgb> >& palettes);
void mos_config_save(const QMap< QString, color_range >& ranges, const QMap< QString, QList<QRgb> >& palettes);

QStringList mos_recent_files();
void mos_add_recent_file(const QString& filepath);
void mos_remove_recent_file(const QString& filepath);

unsigned mos_max_recent_files();

#endif // APPCONFIG_HPP
