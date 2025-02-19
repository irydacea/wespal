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

#include <QTest>

#include "tests.hpp"

#include "defs.hpp"
#include "recentfiles.hpp"
#include "wesnothrc.hpp"

#include <QColorSpace>

QTEST_MAIN(TestMorningStar)
;

namespace {

constexpr unsigned SWATCH_SIZE = 19;
constexpr unsigned SWATCH_COUNT = 9;

// Generated using:
//   utils/unique-colors --ignore-transparent --ordered tests/magenta-palette*
// (plus some hand-formatting for C++, also magenta is a separate array)
constexpr std::array<std::array<unsigned, SWATCH_SIZE>, SWATCH_COUNT> tcSwatches{{
	// magenta-palette-RC-magenta-1-red.png
	{ 0x5E0000, 0xA10000, 0xFF0000, 0x240000, 0x360000, 0x450000, 0x520000,
	  0x6B0000, 0x790000, 0x850000, 0x920000, 0xC00000, 0xD20000, 0xE20000,
	  0xF00000, 0xFF3030, 0xFF6060, 0xFF9191, 0xFFC5C5 },
	// magenta-palette-RC-magenta-2-blue.png
	{ 0x1A2142, 0x222E67, 0x2E419B, 0x131622, 0x15192C, 0x171C35, 0x191F3C,
	  0x1C2449, 0x1D2651, 0x1F2958, 0x202B5F, 0x263478, 0x283882, 0x2A3B8B,
	  0x2C3E93, 0x5565AD, 0x7D89C0, 0xA4ADD3, 0xD0D4E8 },
	// magenta-palette-RC-magenta-3-green.png
	{ 0x244325, 0x3E733F, 0x62B664, 0x0D190E, 0x142615, 0x1A311B, 0x1F3B20,
	  0x294C2A, 0x2E562F, 0x335F34, 0x386839, 0x4A894B, 0x519652, 0x57A158,
	  0x5CAB5E, 0x7FC381, 0x9DD19E, 0xBBDFBC, 0xDBEEDC },
	// magenta-palette-RC-magenta-4-purple.png
	{ 0x36003A, 0x5D0063, 0x93009D, 0x140016, 0x1F0021, 0x28002B, 0x2F0033,
	  0x3D0042, 0x46004A, 0x4C0052, 0x54005A, 0x6F0076, 0x790081, 0x82008B,
	  0x8A0094, 0xA730AF, 0xBB60C2, 0xD091D4, 0xE6C5E9 },
	// magenta-palette-RC-magenta-5-black.png
	{ 0x212121, 0x393939, 0x5A5A5A, 0x0C0C0C, 0x131313, 0x181818, 0x1D1D1D,
	  0x252525, 0x2A2A2A, 0x2F2F2F, 0x333333, 0x444444, 0x4A4A4A, 0x4F4F4F,
	  0x545454, 0x797979, 0x989898, 0xB7B7B7, 0xDADADA },
	// magenta-palette-RC-magenta-6-brown.png
	{ 0x361D0E, 0x5D3218, 0x945027, 0x150B05, 0x1F1108, 0x28150A, 0x30190C,
	  0x3E2110, 0x462612, 0x4D2914, 0x542D16, 0x6F3C1D, 0x7A4220, 0x834722,
	  0x8B4B24, 0xA8714F, 0xBC9278, 0xD0B3A1, 0xE7D7CE },
	// magenta-palette-RC-magenta-7-orange.png
	{ 0x673809, 0xA75505, 0xFF7E00, 0x311E0C, 0x42260B, 0x502D0A, 0x5C330A,
	  0x743D08, 0x814307, 0x8C4907, 0x984E06, 0xC46203, 0xD56A02, 0xE47101,
	  0xF17700, 0xFF9630, 0xFFAE60, 0xFFC791, 0xFFE2C5 },
	// magenta-palette-RC-magenta-8-white.png
	{ 0x666666, 0x999999, 0xE1E1E1, 0x393939, 0x474747, 0x535353, 0x5D5D5D,
	  0x707070, 0x7B7B7B, 0x838383, 0x8D8D8D, 0xB1B1B1, 0xBFBFBF, 0xCBCBCB,
	  0xD6D6D6, 0xE6E6E6, 0xECECEC, 0xF2F2F2, 0xF8F8F8 },
	// magenta-palette-RC-magenta-9-teal.png
	{ 0x114B47, 0x1E8079, 0x30CBC0, 0x061C1B, 0x0A2B28, 0x0D3734, 0x0F413E,
	  0x145550, 0x16605B, 0x196A64, 0x1B746E, 0x249991, 0x27A79E, 0x2AB4AA,
	  0x2DBFB5, 0x57D4CB, 0x7EDED7, 0xA5E8E3, 0xD0F3F0 },
}};

constexpr std::array<unsigned, SWATCH_SIZE> magentaSwatch{
	// magenta-palette.png
	0x8C0051, 0xEC008C, 0xF49AC1, 0x3F0016, 0x55002A, 0x690039, 0x7B0045,
	0x9E005D, 0xB10069, 0xC30074, 0xD6007F, 0xEE3D96, 0xEF5BA1, 0xF172AC,
	0xF287B6, 0xF6ADCD, 0xF8C1D9, 0xFAD5E5, 0xFDE9F1
};

static_assert(magentaSwatch.size() == tcSwatches[0].size());

} // end unnamed namespace

void TestMorningStar::testBuiltinObjects()
{
	using namespace wesnoth;

	const auto colorRangeCount = builtinColorRanges.objectCount();

	const auto& colorRangeStore = builtinColorRanges.objects();
	const auto& colorRangeNames = builtinColorRanges.orderedNames();
	const auto& colorRangeTranslatables = builtinColorRanges.orderedTranslatableNames();
	const auto& colorRangeHandles = builtinColorRanges.orderedObjects();

	QCOMPARE(colorRangeStore.count(), colorRangeCount);
	QCOMPARE(colorRangeNames.count(), colorRangeCount);
	QCOMPARE(colorRangeTranslatables.count(), colorRangeCount);
	QCOMPARE(colorRangeHandles.count(), colorRangeCount);
}

void TestMorningStar::testRecolorAlgorithm()
{
	using namespace wesnoth;

	auto numColorRanges = builtinColorRanges.orderedObjects().count();
	const auto& colorRangeHandles = builtinColorRanges.orderedObjects();

	QCOMPARE_GE(numColorRanges, qsizetype(tcSwatches.size()));

	QVERIFY(builtinPalettes.hasName("magenta"));

	const auto& palMagenta = builtinPalettes["magenta"];

	// NOTE: We only test the first 9 color ranges that were built-in for
	//       Wesnoth 1.2 through 1.14 because we don't currently have the hex
	//       values for the recolored magenta palettes for the newer color
	//       ranges from 1.16+. This will do.
	for (qsizetype i = 0; i < SWATCH_COUNT; ++i) {
		// Generate ordered color map from reference swatch
		ColorMap swatchColorMap;

		// FIXME: ColorRange::applyToPalette() currently forces an alpha of
		//        255 on its output, which causes map values not to match.
		//        The actual recoloring algorithm ignores alpha so this is not
		//        normally an issue.
		for (std::size_t j = 0; j < SWATCH_SIZE; ++j)
			swatchColorMap.insert(magentaSwatch[j], tcSwatches[i][j] + 0xFF000000);

		const auto& colorRange = *colorRangeHandles[i];
		const auto& rcOutput = colorRange.applyToPalette(palMagenta);

		QCOMPARE(rcOutput, swatchColorMap);
	}
}

void TestMorningStar::testWesnothRcImage()
{
	using namespace wesnoth;

	const auto& colorRanges = builtinColorRanges.orderedObjects();
	const auto& colorRangeNames = builtinColorRanges.orderedNames();

	QVERIFY(builtinPalettes.hasName("magenta"));

	const auto& palMagenta = builtinPalettes["magenta"];

	auto pathMagentaSwatch = QFINDTESTDATA("../tests/magenta-palette.png");
	QImage imgMagentaSwatch{pathMagentaSwatch, "PNG"};

	imgMagentaSwatch.convertTo(QImage::Format_ARGB32); // Normalise format

	for (std::size_t i = 0; i < SWATCH_COUNT; ++i)
	{
		const auto& colorRange = *colorRanges[i];

		auto pathColorSwatch = QFINDTESTDATA(
			QString{"../tests/magenta-palette-RC-magenta-%1-%2.png"}
				.arg(i + 1)
				.arg(colorRangeNames[i]));
		QImage imgColorSwatch{pathColorSwatch, "PNG"};

		imgColorSwatch.convertTo(QImage::Format_ARGB32); // Normalise format

		QImage rcOutput = recolorImage(imgMagentaSwatch, colorRange.applyToPalette(palMagenta));

		QCOMPARE(rcOutput, imgColorSwatch);
	}
}

void TestMorningStar::testPaletteSwapImage()
{
	using namespace wesnoth;

	const auto& palMagenta = builtinPalettes["magenta"];
	const auto& colorRangeOrange = builtinColorRanges["orange"];

	// This test primarily focusses on operations on images with non-binary
	// alpha channels. We use reference images with binary alpha only to
	// ensure alpha information isn't lost in recolorImage().

	auto pathMagentaSwatch = QFINDTESTDATA("../tests/magenta-palette.png");
	QImage imgMagentaSwatch{pathMagentaSwatch, "PNG"};

	auto pathOrangeSwatch = QFINDTESTDATA("../tests/magenta-palette-RC-magenta-7-orange.png");
	QImage imgOrangeSwatch{pathOrangeSwatch, "PNG"};

	auto pathAlphaMagentaSwatch = QFINDTESTDATA("../tests/alpha-magenta.png");
	QImage imgAlphaMagentaSwatch{pathAlphaMagentaSwatch, "PNG"};

	auto pathAlphaOrangeSwatch = QFINDTESTDATA("../tests/alpha-magenta-to-orange.png");
	QImage imgAlphaOrangeSwatch{pathAlphaOrangeSwatch, "PNG"};

	// Normalise formats
	imgMagentaSwatch.convertTo(QImage::Format_ARGB32);
	imgOrangeSwatch.convertTo(QImage::Format_ARGB32);
	imgAlphaMagentaSwatch.convertTo(QImage::Format_ARGB32);
	imgAlphaOrangeSwatch.convertTo(QImage::Format_ARGB32);

	const auto& colorMap = colorRangeOrange.applyToPalette(palMagenta);

	QImage rcOutput = recolorImage(imgAlphaMagentaSwatch, colorMap);

	QCOMPARE(rcOutput, imgAlphaOrangeSwatch);
	QCOMPARE_NE(rcOutput, imgOrangeSwatch);

	// Generate reverse color map
	ColorMap reverseColorMap;
	for (const auto& [key, value] : colorMap.asKeyValueRange())
	{
		reverseColorMap.insert(value, key);
	}

	QImage reverseRcOutput = recolorImage(rcOutput, reverseColorMap);

	QCOMPARE(reverseRcOutput, imgAlphaMagentaSwatch);
	QCOMPARE_NE(reverseRcOutput, imgMagentaSwatch);

	// Idempotent color map
	ColorMap sameColorMap;
	for (auto i = colorMap.begin(); i != colorMap.end(); ++i)
	{
		sameColorMap.insert(i.key(), i.key());
	}

	QImage idempotentRcOutput = recolorImage(reverseRcOutput, sameColorMap);

	QCOMPARE(idempotentRcOutput, reverseRcOutput);
	// By this point we've guaranteed alpha information is never lost
	QCOMPARE(idempotentRcOutput, imgAlphaMagentaSwatch);
	QCOMPARE_NE(idempotentRcOutput, imgMagentaSwatch);
}

void TestMorningStar::testColorShiftImage()
{
	auto pathTestInput = QFINDTESTDATA("../tests/shift-test-input.png");
	QImage imgTestInput{pathTestInput, "PNG"};

	imgTestInput.convertTo(QImage::Format_ARGB32);

	auto pathTestReference = QFINDTESTDATA("../tests/shift-test-reference.png");
	QImage imgTestReference{pathTestReference, "PNG"};

	// Strip color space information prior to transform/comparison
	// (see also MosIO::writeImageDeviceAgnostic())
	imgTestInput.setColorSpace({});
	imgTestReference.setColorSpace({});

	QImage imgTestOutput = colorShiftImage(imgTestInput, -228, 90, 164);

	// We cannot use QImage::operator== here because our reference image is
	// produced by Wesnoth's --render option and, surprise surprise, the
	// Wesnoth 1.18 version has a funny quirk where it deliberately ignores
	// any pixels with an alpha value of 0, leaving them untouched. This means
	// Wespal's output, while technically correct, is actually *incorrect*
	// within the specific context of Wesnoth's engine.
	//
	// Qt uses memcmp for operator==, which means that all bits must match. We
	// want to make an explicit exception for alpha 0 pixels, which means we
	// have to compare both sides byte by byte. Yay.

	QCOMPARE(imgTestOutput.format(), imgTestReference.format());
	QCOMPARE(imgTestOutput.format(), imgTestInput.format());

	QCOMPARE(imgTestOutput.size(), imgTestReference.size());
	QCOMPARE(imgTestOutput.size(), imgTestInput.size());

	for (int y = 0; y < imgTestOutput.height(); ++y)
	{
		auto* inRow  = reinterpret_cast<QRgb*>(imgTestInput.scanLine(y));
		auto* outRow = reinterpret_cast<QRgb*>(imgTestOutput.scanLine(y));
		auto* refRow = reinterpret_cast<QRgb*>(imgTestReference.scanLine(y));

		for (int x = 0; x < imgTestOutput.width(); ++x)
		{
			auto inRgba = inRow[x], outRgba = outRow[x], refRgba = refRow[x];

			if (outRgba == refRgba)
				continue;

			// Ensure this is an alpha 0 pixel and that the input and
			// reference pixels are identical. Otherwise something must've
			// gone wrong somewhere, potentially in the test input data.
			QVERIFY2((qAlpha(outRgba) == 0 && qAlpha(refRgba) == 0) &&
					 (inRgba & 0xFFFFFFU) == (refRgba & 0xFFFFFFU),
					 "Output and reference images differ in a way inconsistent with the "
					 "Wesnoth adjust_surface_color() quirk, verify test data!");
		}
	}
}

void TestMorningStar::testColorBlendImage()
{
	auto pathTestInput = QFINDTESTDATA("../tests/blend-test-input.png");
	QImage imgTestInput{pathTestInput, "PNG"};

	imgTestInput.convertTo(QImage::Format_ARGB32);

	auto pathTestReference = QFINDTESTDATA("../tests/blend-test-reference.png");
	QImage imgTestReference{pathTestReference, "PNG"};

	// Strip color space information prior to transform/comparison
	// (see also MosIO::writeImageDeviceAgnostic())
	imgTestInput.setColorSpace({});
	imgTestReference.setColorSpace({});

	QImage imgTestOutput = colorBlendImage(imgTestInput, QColor{127, 89, 32}, 0.54);

	QCOMPARE(imgTestOutput, imgTestReference);
}

void TestMorningStar::testMru()
{
	using namespace MosConfig;

	// Non-standard size on purpose
	MruList subject{7};

	QStringList files = {
		"../tests/magenta-palette-RC-magenta-1-red.png",	// 0
		"../tests/magenta-palette-RC-magenta-2-blue.png",	// 1
		"../tests/magenta-palette-RC-magenta-3-green.png",	// 2
		"../tests/magenta-palette-RC-magenta-4-purple.png",	// 3
		"../tests/magenta-palette-RC-magenta-5-black.png",	// 4
		"../tests/magenta-palette-RC-magenta-6-brown.png",	// 5
		"../tests/magenta-palette-RC-magenta-7-orange.png",	// 6
		"../tests/magenta-palette-RC-magenta-8-white.png",	// 7
		"../tests/magenta-palette-RC-magenta-9-teal.png",	// 8
		"../tests/magenta-palette.png",						// 9
	};

	QString mruBack = QFINDTESTDATA("../tests/magenta-palette-RC-magenta-4-purple.png");
	QString mruBack2 = QFINDTESTDATA("../tests/magenta-palette-RC-magenta-5-black.png");
	QString mruFront = QFINDTESTDATA("../tests/magenta-palette.png");
	QString mruFront2 = QFINDTESTDATA("../tests/magenta-palette-RC-magenta-1-red.png");
	QString mruFront3 = QFINDTESTDATA("../tests/magenta-palette-RC-magenta-9-teal.png");

	for (auto& file : files)
	{
		file = QFINDTESTDATA(file);
		QImage image{file, "PNG"};

		subject.push(file, image);
	}

	QCOMPARE(subject.count(), qMin(files.count(), subject.max()));
	QCOMPARE(subject.back().filePath(), mruBack);
	QCOMPARE(subject.front().filePath(), mruFront);

	subject.push(mruFront2, QImage{mruFront2, "PNG"});

	QCOMPARE(subject.back().filePath(), mruBack2);
	QCOMPARE(subject.front().filePath(), mruFront2);

	// Pushing an existing element just moves it to the top
	subject.push(mruFront3, QImage{mruFront3, "PNG"});

	QStringList newMru = {
		"../tests/magenta-palette-RC-magenta-9-teal.png",	// 0
		"../tests/magenta-palette-RC-magenta-1-red.png",	// 1
		"../tests/magenta-palette.png",						// 2
		"../tests/magenta-palette-RC-magenta-8-white.png",	// 3
		"../tests/magenta-palette-RC-magenta-7-orange.png",	// 4
		"../tests/magenta-palette-RC-magenta-6-brown.png",	// 5
		"../tests/magenta-palette-RC-magenta-5-black.png",	// 6
	};

	qsizetype i = 0;

	for (auto it = subject.begin(); it != subject.end(); ++it, ++i)
	{
		QCOMPARE_LT(i, newMru.count());
		QCOMPARE(it->filePath(), QFINDTESTDATA(newMru[i]));
	}

	// Re-pushing the top is a no-op
	subject.push(mruFront3, QImage{});
	subject.push(mruFront3, QImage{});
	subject.push(mruFront3, QImage{});

	QCOMPARE_NE(subject.begin(), subject.begin() + 1);
	QCOMPARE((subject.begin() + 1)->filePath(), QFINDTESTDATA(newMru[1]));
}

void TestMorningStar::testUniqueColorsFromImage()
{
	using namespace wesnoth;

	const auto& palMagenta = builtinPalettes["magenta"];

	ColorSet reference{palMagenta.begin(), palMagenta.end()};

	// The transparent portions of the test image have a colour value of
	// exactly 255,255,255
	reference << 0xFFFFFFU;

	auto pathMagentaSwatch = QFINDTESTDATA("../tests/magenta-palette.png");
	QImage imgMagentaSwatch{pathMagentaSwatch, "PNG"};

	const auto& result = uniqueColorsFromImage(imgMagentaSwatch);

	QCOMPARE(result, reference);
}

void TestMorningStar::testWriteBase64()
{
	using namespace wesnoth;

	auto pathMagentaSwatch = QFINDTESTDATA("../tests/magenta-palette.png");
	QImage imgMagentaSwatch{pathMagentaSwatch, "PNG"};

	const auto& base64 = MosIO::writeBase64Png(imgMagentaSwatch);

	QImage imgDecoded;
	imgDecoded.loadFromData(QByteArray::fromBase64(base64.toUtf8()), "PNG");

	QVERIFY(imgMagentaSwatch.isNull() == false);
	QVERIFY(imgDecoded.isNull() == false);

	QCOMPARE(imgMagentaSwatch, imgDecoded);
}
