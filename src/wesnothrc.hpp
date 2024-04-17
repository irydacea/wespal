/*
 * Wespal (codename Morning Star) - Wesnoth assets recoloring tool
 *
 * Copyright (C) 2003 - 2024 by The Battle for Wesnoth Project <www.wesnoth.org>
 * Copyright (C) 2010 - 2024 by Iris Morelle <iris@irydacea.me>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#pragma once

#include "colortypes.hpp"

#include <QImage>
#include <QString>

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
class ColorRange
{
public:
	/**
	 * Constructor, which expects four reference RGB colors.
	 *
	 * @param mid Average color shade.
	 * @param max Maximum (highlight) color shade
	 * @param min Minimum color shade
	 * @param rep High-contrast icon color
	 */
	ColorRange(QRgb mid = 0x808080,
			   QRgb max = 0xFFFFFF,
			   QRgb min = 0x000000,
			   QRgb rep = 0x808080)
		: mid_(mid)
		, max_(max)
		, min_(min)
		, rep_(rep)
	{
	}

	/**
	 * Average color shade.
	 */
	QRgb mid() const
	{
		return mid_;
	}

	/**
	 * Sets the average color shade.
	 */
	void setMid(QRgb mid)
	{
		mid_ = mid;
	}

	/**
	 * Maximum color shade.
	 */
	QRgb max() const
	{
		return max_;
	}

	/**
	 * Sets the maximum color shade.
	 */
	void setMax(QRgb max)
	{
		max_ = max;
	}

	/**
	 * Minimum color shade.
	 */
	QRgb min() const
	{
		return min_;
	}

	/**
	 * Sets the minimum color shade.
	 */
	void setMin(QRgb min)
	{
		min_ = min;
	}

	/**
	 * High-contrast icon color.
	 */
	QRgb rep() const
	{
		return rep_;
	}

	/**
	 * Sets the high-contrast icon color.
	 */
	void setRep(QRgb rep)
	{
		rep_ = rep;
	}

	/**
	 * Transforms a source palette using this color_range object.
	 *
	 * @param palette Source palette.
	 *
	 * @return A color map, where the keys are source palette items, and the
	 *         values mapped to them are the result of applying a color range
	 *         transform to them.
	 */
	ColorMap applyToPalette(const ColorList& palette) const;

private:
	QRgb mid_ , max_ , min_, rep_;
};

inline bool operator<(const ColorRange& a, const ColorRange& b)
{
	if (a.mid() != b.mid())
		return a.mid() < b.mid();
	if (a.max() != b.max())
		return a.max() < b.max();
	return a.min() < b.min();
}

inline bool operator==(const ColorRange& a, const ColorRange& b)
{
	return a.mid() == b.mid() &&
		   a.max() == b.max() &&
		   a.min() == b.min();
}

/**
 * Converts a source palette using the specified color_range object.
 * This holds the main interface for range-based recoloring.
 *
 * @param new_rgb      Specifies parameters for the conversion.
 *
 * @param old_rgb      Source palette.
 *
 * @return A color map, where the keys are source palette items, and the
 *         values mapped to them are the result of applying a color range
 *         transform to them.
 */
ColorMap applyColorRange(const ColorRange& colorRange,
						 const ColorList& palette);

/**
 * Generates a color map from two palettes.
 *
 * @param srcPalette   Source palette.
 *
 * @param newPalette   New palette.
 *
 * @return A color map, where the keys are source palette items, and the
 *         values mapped to them are taken from their analogous index in th
 *         new palette.
 *
 * @note If srcPalette and newPalette differ in size, the operation uses at
 *       most the number of colors in the smallest palette.
 */
ColorMap generateColorMap(const ColorList& srcPalette,
						  const ColorList& newPalette);

/**
 * Generates Wesnoth Markup from a color range object.
 *
 * @param name         Color range name to be used for its WML id and
 *                     human-friendly name in English.
 *
 * @param range        Color range object.
 *
 * @return A string containing WML code including a [color_range] tag.
 */
QString wmlFromColorRange(const QString& name,
						  const ColorRange& range);

/**
 * Generates Wesnoth Markup from a color list.
 *
 * @param name         Color palette name to be used for its WML id and
 *                     human-friendly name in English.
 *
 * @param palette      Color palette object.
 *
 * @return A string containing WML code including a [color_palette] tag.
 */
QString wmlFromColorList(const QString& name,
						 const ColorList& palette);

/**
 * Extracts a set of unique colors in an image.
 *
 * @param input        Input image.
 *
 * @return An ordered set of unique colors found in the input, with alpha
 *         values set to zero.
 */
ColorSet uniqueColorsFromImage(const QImage& input);

/**
 * Recolors a QImage using the specified color map.
 *
 * The color map provided may be obtained by e.g. calling ColorRange::apply()
 * on an appropriate palette.
 *
 * @param input        Input image.
 *
 * @param colorMap     A color map to use for transforming the image.
 *
 * @return A recolored image, always in ARGB32 format regardless of the input
 *         format.
 */
QImage recolorImage(const QImage& input,
					const ColorMap& colorMap);

namespace MosIO {

/**
 * Writes a QImage to disk as a PNG file.
 *
 * @param input        Input image (see notes).
 * @param fileName     Image file name.
 *
 * @note @a input is assumed to be in ARGB32 format, although this is not
 *       a particularly significant assumption anyway. More importantly, this
 *       function may MODIFY its input to ensure that its color space
 *       configuration is correct for the intended output format.
 */
bool writePng(QImage& input, const QString& fileName);

/**
 * Writes a palette to disk in GIMP palette (.gpl) format.
 *
 * @param palette      Input palette.
 * @param fileName     Palette file name.
 */
bool writeGimpPalette(const ColorList& palette,
					  const QString& fileName,
					  const QString& paletteName);

} // end namespace MosIO
