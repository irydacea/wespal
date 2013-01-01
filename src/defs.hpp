//
// codename Morning Star
//
// Copyright (C) 2010 - 2013 by Ignacio Riquelme Morelle <shadowm2006@gmail.com>
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

#ifndef DEFS_HPP
#define DEFS_HPP

#include "wesnothrc.hpp"

#include <QStringList>

// Definitions of built-in (Wesnoth mainline)
// color ranges and palettes.

/** Built-in color palettes. */
extern QMap< QString, QList<QRgb> > mosBuiltinColorPalettes;
/** Names of the built-in color palettes, sorted for presentation purposes. */
extern QStringList mosOrderedPaletteNames;

/** Built-in color ranges. */
extern QMap< QString, color_range > mosBuiltinColorRanges;
/** Names of the built-in color ranges, sorted for presentation purposes. */
extern QStringList mosOrderedRangeNames;

#endif // DEFS_HPP
