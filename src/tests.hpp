/*
 * Wespal (codename Morning Star) - Wesnoth assets recoloring tool
 *
 * Copyright (C) 2025 by Iris Morelle <iris@irydacea.me>
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

#include <QTest>

class TestMorningStar: public QObject
{
	Q_OBJECT

private slots:
	void testMru();
	void testBuiltinObjects();
	void testRecolorAlgorithm();
	void testWesnothRcImage();
	void testPaletteSwapImage();
	void testColorShiftImage();
	void testColorBlendImage();
	void testUniqueColorsFromImage();
	void testWriteBase64();
};
