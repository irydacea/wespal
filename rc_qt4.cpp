//
// codename Morning Star
// src/rc_qt4.cpp - Qt4 painting device recoloring implementation
//
// Copyright (C) 2008 - 2012 by Ignacio Riquelme Morelle <shadowm2006@gmail.com>
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

#include "rc_qt4.hpp"

namespace {
	inline bool rgb_equals(QRgb c1, QRgb c2)
	{
		// FIXME: optimization
		return qRed(c1) == qRed(c2) && qGreen(c1) == qGreen(c2) && qBlue(c1) == qBlue(c2);
	}

	inline QRgb rc_single_color(QRgb newcolor, QRgb oldcolor)
	{
		// FIXME: optimization
		return qRgba(qRed(newcolor), qGreen(newcolor), qBlue(newcolor), qAlpha(oldcolor));
	}
}

bool rc_image(const QImage &input, QImage &output, const rc_map &cvt_map)
{
	output = input;

	// Always use ARGB
	if(output.format() != QImage::Format_ARGB32)
		output.convertToFormat(QImage::Format_ARGB32);

	int x, y;
	for(y = 0; y < output.height(); ++y) {
		for (x = 0; x < output.width(); ++x) {
			QRgb color = output.pixel(x, y);
			foreach(QRgb key, cvt_map.keys()) {
				if(rgb_equals(key, color)) {
					output.setPixel(x, y, rc_single_color(cvt_map[key], color));
				}
			}
		}
	}

	return true;
}
