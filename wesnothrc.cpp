//
// wesnoth-tc - Wesnoth Team-Colorizer
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

QMap<QRgb, QRgb> recolor_range(const color_range& new_range, const QVector<QRgb>& old_rgb){
	QMap<QRgb, QRgb> map_rgb;

	short new_red  = (new_range.mid() & 0x00FF0000)>>16;
	short new_green= (new_range.mid() & 0x0000FF00)>>8;
	short new_blue = (new_range.mid() & 0x000000FF);
	short max_red  = (new_range.max() & 0x00FF0000)>>16;
	short max_green= (new_range.max() & 0x0000FF00)>>8 ;
	short max_blue = (new_range.max() & 0x000000FF)    ;
	short min_red  = (new_range.min() & 0x00FF0000)>>16;
	short min_green= (new_range.min() & 0x0000FF00)>>8 ;
	short min_blue = (new_range.min() & 0x000000FF)    ;

	// Map first color in vector to exact new color
	QRgb temp_rgb = old_rgb.empty() ? 0 : old_rgb[0];
	short old_r = (temp_rgb & 0x00FF0000)>>16;
	short old_g = (temp_rgb & 0x0000FF00)>>8;
	short old_b = (temp_rgb & 0x000000FF);
	short reference_avg = ((old_r + old_g + old_b) / 3);

	for(QVector<QRgb>::const_iterator temp_rgb2 = old_rgb.begin();
		  temp_rgb2 != old_rgb.end(); ++temp_rgb2)
	{
		short old_r=((*temp_rgb2) & 0X00FF0000)>>16;
		short old_g=((*temp_rgb2) & 0X0000FF00)>>8;
		short old_b=((*temp_rgb2) & 0X000000FF);

		const short old_avg = ((old_r + old_g +  old_b) / 3);
		 // Calculate new color
		QRgb new_r, new_g, new_b;

		if(reference_avg && old_avg <= reference_avg){
			float old_rat = static_cast<float>(old_avg)/reference_avg;
			new_r = QRgb(old_rat * new_red   + (1 - old_rat) * min_red);
			new_g = QRgb(old_rat * new_green + (1 - old_rat) * min_green);
			new_b = QRgb(old_rat * new_blue  + (1 - old_rat) * min_blue);
		}else if(255 - reference_avg){
			float old_rat = (255.0f - static_cast<float>(old_avg)) /
				(255.0f - reference_avg);

			new_r = static_cast<QRgb>(old_rat * new_red   + (1 - old_rat) * max_red);
			new_g = static_cast<QRgb>(old_rat * new_green + (1 - old_rat) * max_green);
			new_b = static_cast<QRgb>(old_rat * new_blue  + (1 - old_rat) * max_blue);
		}else{
			new_r = new_g = new_b = 0; // Suppress warning
			Q_ASSERT(false);
			// Should never get here.
			// Would imply old_avg > reference_avg = 255
		 }

		if(new_r>255) new_r=255;
		if(new_g>255) new_g=255;
		if(new_b>255) new_b=255;

		map_rgb[*temp_rgb2] = (new_r << 16) + (new_g << 8) + (new_b);
	}

	return map_rgb;
}

// kate: indent-mode normal; encoding utf-8; space-indent off; indent-width 4;
