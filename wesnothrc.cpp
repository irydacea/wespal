//
// codename Morning Star
// src/color_range.cpp - Recoloring mechanism (implementation)
//
// Copyright (C) 2003-2008 by the Battle for Wesnoth Project <www.wesnoth.org>
// Copyright (C) 2010 by Ignacio R. Morelle <shadowm@wesnoth.org>
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

/** @file color_range.cpp
 * Generate ranges of colors, and color palettes.
 * Used e.g. to color HP, XP.
 */

#include "wesnothrc.hpp"

QMap<QRgb, QRgb> recolor_range(const color_range& new_range, const QList<QRgb>& old_rgb){
	QMap<QRgb, QRgb> map_rgb;

	short new_red  = qRed(new_range.mid());
	short new_green= qGreen(new_range.mid());
	short new_blue = qBlue(new_range.mid());

	short max_red  = qRed(new_range.max());
	short max_green= qGreen(new_range.max());
	short max_blue = qBlue(new_range.max());

	short min_red  = qRed(new_range.min());
	short min_green= qGreen(new_range.min());
	short min_blue = qBlue(new_range.min());

	// Map first color in vector to exact new color
	QRgb temp_rgb = old_rgb.empty() ? 0 : old_rgb[0];
	short old_r = qRed(temp_rgb);
	short old_g = qGreen(temp_rgb);
	short old_b = qBlue(temp_rgb);
	short reference_avg = ((old_r + old_g + old_b) / 3);

	for(QList<QRgb>::const_iterator temp_rgb2 = old_rgb.begin();
		  temp_rgb2 != old_rgb.end(); ++temp_rgb2)
	{
		short old_r = qRed(*temp_rgb2);
		short old_g = qGreen(*temp_rgb2);
		short old_b = qBlue(*temp_rgb2);

		const short old_avg = ((old_r + old_g + old_b) / 3);
		 // Calculate new color
		QRgb new_r, new_g, new_b;

		if(reference_avg && old_avg <= reference_avg) {
			float old_rat = static_cast<float>(old_avg)/reference_avg;
			new_r = QRgb(old_rat * new_red   + (1 - old_rat) * min_red);
			new_g = QRgb(old_rat * new_green + (1 - old_rat) * min_green);
			new_b = QRgb(old_rat * new_blue  + (1 - old_rat) * min_blue);
		} else if(255 - reference_avg) {
			float old_rat = (255.0f - static_cast<float>(old_avg)) /
				(255.0f - reference_avg);

			new_r = static_cast<QRgb>(old_rat * new_red   + (1 - old_rat) * max_red);
			new_g = static_cast<QRgb>(old_rat * new_green + (1 - old_rat) * max_green);
			new_b = static_cast<QRgb>(old_rat * new_blue  + (1 - old_rat) * max_blue);
		} else {
			new_r = new_g = new_b = 0; // Suppress warning
			Q_ASSERT(false);
			// Should never get here.
			// Would imply old_avg > reference_avg = 255
		 }

		if(new_r > 255) new_r=255;
		if(new_g > 255) new_g=255;
		if(new_b > 255) new_b=255;

		map_rgb[*temp_rgb2] = qRgba(new_r, new_g, new_b, 0);
	}

	return map_rgb;
}

QMap<QRgb, QRgb> recolor_palettes(const QList<QRgb> &key, const QList<QRgb> &new_values)
{
	rc_map map;

	for(int n = 0; n < qMin(key.count(), new_values.count()); ++n) {
		map[key.at(n)] = new_values.at(n);
	}

	return map;
}

// kate: indent-mode normal; encoding utf-8; space-indent off; indent-width 4;
