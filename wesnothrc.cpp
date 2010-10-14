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

#include <cassert>
#include <cstdlib>
#include <set>
#include <string>
#include <vector>

namespace {
	void remove_surrounding_quotes(std::string& str)
	{
		std::string::size_type p;

		do {
			p = str.find('\"');
			if (p != str.npos)
				str.erase(p, 1);
		} while (p != str.npos);
	}

	std::vector<std::string> split(std::string const &val, char c)
	{
		std::string sval = val;
		remove_surrounding_quotes(sval);

		std::vector<std::string> res;

		std::string::const_iterator i1 = sval.begin();
		std::string::const_iterator i2 = sval.begin();

		while (i2 != sval.end()) {
			if (*i2 == c) {
				std::string new_val(i1, i2);
				if (!new_val.empty())
					res.push_back(new_val);
				++i2;
				i1 = i2;
			} else {
				++i2;
			}
		}

		std::string new_val(i1, i2);
		if (!new_val.empty())
			res.push_back(new_val);

		return res;
	}
}

QMap<QRgb, QRgb> recolor_range(const color_range& new_range, const std::vector<uint32_t>& old_rgb){
	QMap<QRgb, QRgb> map_rgb;

	uint16_t new_red  = (new_range.mid() & 0x00FF0000)>>16;
	uint16_t new_green= (new_range.mid() & 0x0000FF00)>>8;
	uint16_t new_blue = (new_range.mid() & 0x000000FF);
	uint16_t max_red  = (new_range.max() & 0x00FF0000)>>16;
	uint16_t max_green= (new_range.max() & 0x0000FF00)>>8 ;
	uint16_t max_blue = (new_range.max() & 0x000000FF)    ;
	uint16_t min_red  = (new_range.min() & 0x00FF0000)>>16;
	uint16_t min_green= (new_range.min() & 0x0000FF00)>>8 ;
	uint16_t min_blue = (new_range.min() & 0x000000FF)    ;

	// Map first color in vector to exact new color
	uint32_t temp_rgb = old_rgb.empty() ? 0 : old_rgb[0];
	uint16_t old_r = (temp_rgb & 0x00FF0000)>>16;
	uint16_t old_g = (temp_rgb & 0x0000FF00)>>8;
	uint16_t old_b = (temp_rgb & 0x000000FF);
	uint16_t reference_avg = (( old_r + old_g + old_b) / 3);

	for(std::vector< uint32_t >::const_iterator temp_rgb2 = old_rgb.begin();
		  temp_rgb2 != old_rgb.end(); ++temp_rgb2)
	{
		uint16_t old_r=((*temp_rgb2) & 0X00FF0000)>>16;
		uint16_t old_g=((*temp_rgb2) & 0X0000FF00)>>8;
		uint16_t old_b=((*temp_rgb2) & 0X000000FF);

		const uint16_t old_avg = (( old_r + old_g +  old_b) / 3);
		 // Calculate new color
		uint32_t new_r, new_g, new_b;

		if(reference_avg && old_avg <= reference_avg){
			float old_rat = static_cast<float>(old_avg)/reference_avg;
			new_r=uint32_t( old_rat * new_red   + (1 - old_rat) * min_red);
			new_g=uint32_t( old_rat * new_green + (1 - old_rat) * min_green);
			new_b=uint32_t( old_rat * new_blue  + (1 - old_rat) * min_blue);
		}else if(255 - reference_avg){
			float old_rat = (255.0f - static_cast<float>(old_avg)) /
				(255.0f - reference_avg);

			new_r=static_cast<uint32_t>( old_rat * new_red   + (1 - old_rat) * max_red);
			new_g=static_cast<uint32_t>( old_rat * new_green + (1 - old_rat) * max_green);
			new_b=static_cast<uint32_t>( old_rat * new_blue  + (1 - old_rat) * max_blue);
		}else{
			new_r=0; new_g=0; new_b=0; // Suppress warning
			assert(false);
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

//! Convert comma separated string into rgb values.
std::vector<uint32_t> string2rgb(std::string s){
	std::vector<uint32_t> out;
	std::vector<std::string> rgb_vec = split(s, ',');
	std::vector<std::string>::iterator c = rgb_vec.begin();
	while(c!=rgb_vec.end())
	{
		uint32_t rgb_hex;
		if(c->length() != 6)
		{
			// integer triplets, e.g. white="255,255,255"
			rgb_hex =  (0x00FF0000 & ((lexical_cast<int>(*c++))<<16)); //red
			if(c!=rgb_vec.end())
			{
				rgb_hex += (0x0000FF00 & ((lexical_cast<int>(*c++))<<8)); //green
				if(c!=rgb_vec.end())
				{
					rgb_hex += (0x000000FF & ((lexical_cast<int>(*c++))<<0)); //blue
				}
			}
		} else {
			// hexadecimal format, e.g. white="FFFFFF"
			char* endptr;
			rgb_hex = (0x00FFFFFF & strtol(c->c_str(), &endptr, 16));
			if (*endptr != '\0') {
				throw bad_lexical_cast();
			}
			c++;
		}
		out.push_back(rgb_hex);
	}
	return(out);
}

// kate: indent-mode normal; encoding utf-8; space-indent off; indent-width 4;
