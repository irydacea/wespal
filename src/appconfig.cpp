/*
 * Wespal (codename Morning Star) - Wesnoth assets recoloring tool
 *
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

#include "appconfig.hpp"

#include <QBuffer>
#include <QSettings>
#include <QMessageBox>

namespace MosConfig {

Manager::Manager()
	: imageFilesMru_()
	, customColorRanges_()
	, customPalettes_()
	, mainWindowSize_()
	, previewBackgroundColor_()
{
	QSettings qs;

	//
	// Workspace configuation
	//

	mainWindowSize_ = qs.value("mainwindow_size").toSize();

	previewBackgroundColor_ = qs.value("preview_background").toString();

	imageViewMode_ = qs.value("preview/mode", ImageViewVSplit).value<ImageViewMode>();

	//
	// User-defined color ranges
	//

	const int numRanges = qs.beginReadArray("color_ranges");

	for (int i = 0; i < numRanges; ++i)
	{
		qs.setArrayIndex(i);

		auto id = qs.value("id").toString();
		auto mid = qs.value("avg").toUInt();
		auto max = qs.value("max").toUInt();
		auto min = qs.value("min").toUInt();

		ColorRange colorRange{mid, max, min};

		customColorRanges_.insert(id, colorRange);
	}

	qs.endArray();

	//
	// User-defined color palettes
	//

	const int numPals = qs.beginReadArray("palettes");

	for (int i = 0; i < numPals; ++i)
	{
		qs.setArrayIndex(i);

		// TODO: use QStringList variant instead

		auto id = qs.value("id").toString();
		auto values = qs.value("values").toString().split(',', Qt::SkipEmptyParts);

		ColorList palette;
		palette.reserve(values.count());

		for (const auto& value : values)
			palette.emplaceBack(value.toUInt());

		customPalettes_.insert(id, palette);
	}

	qs.endArray();

	//
	// Recent files
	//

	const int numRecentFiles = qs.beginReadArray("recent_files");

	for (int i = numRecentFiles - 1; i >= 0; --i)
	{
		qs.setArrayIndex(i);

		auto path = qs.value("path").toString();
		auto thumbnailBase64 = qs.value("thumbnail").toString();

		imageFilesMru_.push(path, thumbnailBase64);
	}

	qs.endArray();
}

void Manager::setMainWindowSize(const QSize& size)
{
	QSettings qs;

	mainWindowSize_ = size;

	qs.setValue("mainwindow_size", size);
}

void Manager::setPreviewBackgroundColor(const QString& previewBackgroundColor)
{
	QSettings qs;

	previewBackgroundColor_ = previewBackgroundColor;

	qs.setValue("preview_background", previewBackgroundColor);
}

void Manager::setImageViewMode(ImageViewMode imageViewMode)
{
	QSettings qs;

	imageViewMode_ = imageViewMode;

	qs.setValue("preview/mode", imageViewMode);
}

void Manager::setCustomColorRanges(const QMap<QString, ColorRange>& colorRanges)
{
	QSettings qs;
	int i = 0;

	customColorRanges_ = colorRanges;

	qs.beginWriteArray("color_ranges");

	for (const auto& [id, colorRange] : customColorRanges_.asKeyValueRange())
	{
		qs.setArrayIndex(i);

		qs.setValue("id", id);
		qs.setValue("avg", colorRange.mid());
		qs.setValue("max", colorRange.max());
		qs.setValue("min", colorRange.min());

		++i;
	}

	qs.endArray();
}

void Manager::setCustomPalettes(const QMap<QString, ColorList>& palettes)
{
	QSettings qs;
	int i = 0;

	customPalettes_ = palettes;

	qs.beginWriteArray("palettes");

	for (const auto& [id, palette] : customPalettes_.asKeyValueRange())
	{
		qs.setArrayIndex(i);

		// TODO: use QStringList variant instead

		QString colorList;

		for (auto color : palette)
		{
			if (!colorList.isEmpty())
				colorList += ',';
			colorList += QString::number(color);
		}

		qs.setValue("id", id);
		qs.setValue("values", colorList);

		++i;
	}

	qs.endArray();
}

void Manager::addRecentFile(const QString& filePath, const QImage& image)
{
	QSettings qs;
	int i = 0;

	imageFilesMru_.push(filePath, image);

	qs.beginWriteArray("recent_files");

	for (const auto& entry : imageFilesMru_)
	{
		qs.setArrayIndex(i);

		QBuffer thumbnail;

		thumbnail.open(QIODevice::WriteOnly);
		entry.thumbnail().save(&thumbnail, "PNG");

		qs.setValue("path", entry.filePath());
		qs.setValue("thumbnail", thumbnail.data().toBase64());

		++i;
	}

	qs.endArray();
}

void Manager::clearRecentFiles()
{
	QSettings qs;

	imageFilesMru_.clear();

	qs.beginWriteArray("recent_files");
	qs.endArray();
}

} // end namespace MosConfig
