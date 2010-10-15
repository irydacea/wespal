//
// wesnoth-tc - Wesnoth Team-Colorizer
// src/color_range.hpp - Recoloring mechanism (interface)
//
// Copyright (C) 2003-2008 by the Battle for Wesnoth Project <www.wesnoth.org>
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

/** @file wesnothrcc.hpp */

#ifndef WESNOTHRC_HPP_INCLUDED
#define WESNOTHRC_HPP_INCLUDED

#include <QColor>
#include <QMap>
#include <QString>
#include <QVector>

/** Convert comma separated string into rgb values. */
QVector<QRgb> string2rgb(QString s);

/**
 * A color range definition is made of four reference RGB colors, used
 * for calculating conversions from a source/key palette.
 *
 *   1) The average shade of a unit's team-color portions
 *      (default: gray #808080)
 *   2) The maximum highlight shade of a unit's team-color portions
 *      (default: white)
 *   3) The minimum shadow shade of a unit's team-color portions
 *      (default: black)
 *   4) A plain high-contrast color, used for the markers on the mini-ma
 *      (default: same as the provided average shade, or gray #808080)
 *
 * The first three reference colors are used for converting a source palette
 * with the external recolor_range() method.
 *
 * In this implementation, the fourth reference color is unsupported as
 * it is not required for wesnoth-tc's functionality.
 */
class color_range
{
public:
	/**
	 * Copy constructor.
	 */
	color_range(const color_range& o)
			: mid_(o.mid_)
			, max_(o.max_)
			, min_(o.min_)
	{}

	/**
	 * Constructor, which expects three reference RGB colors.
	 * @param mid Average color shade.
	 * @param max Maximum (highlight) color shade
	 * @param min Minimum color shade
	 */
	color_range(
		QRgb mid,
		QRgb max = 0x00FFFFFF,
		QRgb min = 0x00000000)
			: mid_(mid)
			, max_(max)
			, min_(min)
	{}

	/** Default constructor. */
	color_range() : mid_(0x00808080), max_(0x00FFFFFF), min_(0x00000000)
	{}

	/** Average color shade. */
	QRgb mid() const{ return mid_; }
	/** Maximum color shade. */
	QRgb max() const{ return max_; }
	/** Minimum color shade. */
	QRgb min() const{ return min_; }

	bool operator<(const color_range& b) const
	{
		if(mid_ != b.mid()) {
			return(mid_ < b.mid());
		}
		if(max_ != b.max()) {
			return(max_ < b.max());
		}
		return(min_ < b.min());
	}

	bool operator==(const color_range& b) const
	{
		return(mid_ == b.mid() && max_ == b.max() && min_ == b.min());
	}

	color_range& operator=(const color_range& o)
	{
		if(&o != this) {
			mid_ = o.mid_;
			max_ = o.max_;
			min_ = o.min_;
		}

		return *this;
	}

private:
	QRgb mid_ , max_ , min_;
};

typedef QMap<QRgb, QRgb> rc_map;

/**
 * Converts a source palette using the specified color_range object.
 * This holds the main interface for range-based recoloring.
 * @param new_rgb Specifies parameters for the conversion.
 * @param old_rgb Source palette.
 * @return A STL map of colors, with the keys being source palette elements, and the values
 *         are the result of applying the color range conversion on it.
 */
rc_map recolor_range(const color_range& new_rgb, const QList<QRgb>& old_rgb);

#endif

// kate: indent-mode normal; encoding utf-8; space-indent off; indent-width 4;
