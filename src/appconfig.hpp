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

#pragma once

#include "recentfiles.hpp"
#include "wesnothrc.hpp"

#include <QSize>

// We currently do not support setting the app color scheme on platforms other
// than macOS due to lack of support on other platforms. In particular, on
// Windows, app color scheme hint support requires using the icky Windows 11
// style engine, which we INTENTIONALLY do not use.
//
// TODO: Potentially work around these limitations by using Fusion whenever
// the color scheme hint is unsupported and the app color scheme is not set to
// the OS default, then set our own color theme.
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0) && defined(Q_OS_MACOS)
#	define WESPAL_UI_SUPPORTS_APP_COLOR_SCHEME
#endif

namespace MosConfig {

Q_NAMESPACE

enum ImageViewMode
{
	ImageViewVSplit,
	ImageViewHSplit,
	ImageViewSwipe,
	ImageViewOnionSkin,
	ImageViewSize,
};

Q_ENUM_NS(ImageViewMode)

enum AppColorScheme
{
	AppColorSchemeOSDefault,
	AppColorSchemeOSDark = -1,
	AppColorSchemeOSLight = -2,
};

Q_ENUM_NS(AppColorScheme)

class Manager
{
public:
	/**
	 * Retrieves the current config instance.
	 */
	static Manager& instance()
	{
		static Manager configManager;
		return configManager;
	}

	// original character do not steal
	Manager(const Manager&) = delete;
	Manager(Manager&&) = delete;
	Manager& operator=(const Manager&) = delete;
	Manager& operator=(Manager&&) = delete;

	/**
	 * Retrieves the current recent files.
	 */
	const MruList& recentFiles() const
	{
		return imageFilesMru_;
	}

	/**
	 * Adds a new recent file entry.
	 *
	 * @param filePath       File path.
	 * @param image          Image contents of the file which will be used for
	 *                       generating a thumbnail.
	 */
	void addRecentFile(const QString& filePath, const QImage& image);

	/**
	 * Clears the recent files list.
	 */
	void clearRecentFiles();

	// TODO
	//void removeRecentFile(const QString& filePath);

	/**
	 * Retrieves the list of custom color ranges.
	 */
	const QMap<QString, ColorRange>& customColorRanges() const
	{
		return customColorRanges_;
	}

	/**
	 * Sets the list of custom color ranges.
	 */
	void setCustomColorRanges(const QMap<QString, ColorRange>& colorRanges);

	//void addCustomColorRange(const QString& name, const ColorRange& colorRange);

	//void deleteCustomColorRange(const QString& name);

	/**
	 * Retrieves the list of custom palettes.
	 */
	const QMap<QString, ColorList>& customPalettes() const
	{
		return customPalettes_;
	}

	/**
	 * Sets the list of custom palettes.
	 */
	void setCustomPalettes(const QMap<QString, ColorList>& palettes);

	//void addCustomPalette(const QString& name, const ColorList& palette);

	//void deleteCustomPalette(const QString& palette);

	/**
	 * Retrieves whether the main window size should be remembered.
	 */
	bool rememberMainWindowSize() const
	{
		return rememberMainWindowSize_;
	}

	/**
	 * Sets whether the main window size should be remembered.
	 */
	void setRememberMainWindowSize(bool remember);

	/**
	 * Retrieves the saved main window size.
	 */
	const QSize& mainWindowSize() const
	{
		return mainWindowSize_;
	}

	/**
	 * Sets the saved main window size.
	 */
	void setMainWindowSize(const QSize& size);

	/**
	 * Retrieves the default preview zoom level.
	 *
	 * The zoom value is a numeric factor (e.g. 1.0x, 2.0x) rather than a
	 * percentage.
	 */
	qreal defaultZoom() const
	{
		return defaultZoom_;
	}

	/**
	 * Sets the default preview zoom level.
	 *
	 * The zoom value is a numeric factor (e.g. 1.0x, 2.0x) rather than a
	 * percentage.
	 */
	void setDefaultZoom(qreal zoom);

	/**
	 * Retrieves the preview background color.
	 */
	const QString& previewBackgroundColor() const
	{
		return previewBackgroundColor_;
	}

	/**
	 * Sets the preview background color.
	 */
	void setPreviewBackgroundColor(const QString& previewBackgroundColor);

	/**
	 * Retrieves whether the image view mode should be remembered.
	 */
	bool rememberImageViewMode() const
	{
		return rememberImageViewMode_;
	}

	/**
	 * Sets whether the image view mode should be remembered.
	 */
	void setRememberImageViewMode(bool remember);

	/**
	 * Retrieves the image view mode.
	 */
	ImageViewMode imageViewMode() const
	{
		return imageViewMode_;
	}

	/**
	 * Sets the image view mode.
	 */
	void setImageViewMode(ImageViewMode imageViewMode);

	/**
	 * Returns which OS color scheme should be used for the UI.
	 */
	AppColorScheme appColorScheme() const
	{
		return appColorScheme_;
	}

	/**
	 * Sets the OS color scheme that should be used for the UI.
	 */
	void setAppColorScheme(AppColorScheme scheme);

	/**
	 * Returns whether PNG writer functions should include the Software comment.
	 */
	bool pngVanityPlate() const
	{
		return pngVanityPlate_;
	}

	/**
	 * Sets whether PNG writer functions should include the Software comment.
	 *
	 * If @a true, a PNG tEXt chunk is included in PNG files created by Wespal
	 * stating the software name and version used to generate them.
	 */
	void setPngVanityPlate(bool enable);

private:
	Manager();

	/**
	 * Applies changes to the UI color scheme.
	 *
	 * This function does nothing if changing the UI color scheme is
	 * unsupported by the target platform.
	 */
	void applyAppColorScheme();

	MruList imageFilesMru_;
	QMap<QString, ColorRange> customColorRanges_;
	QMap<QString, ColorList> customPalettes_;
	bool rememberMainWindowSize_;
	QSize mainWindowSize_;
	qreal defaultZoom_;
	QString previewBackgroundColor_;
	bool rememberImageViewMode_;
	ImageViewMode imageViewMode_;
	AppColorScheme appColorScheme_;
	bool pngVanityPlate_;
};

inline Manager& current()
{
	return Manager::instance();
}

} // end namespace MosConfig

inline MosConfig::Manager& MosCurrentConfig()
{
	return MosConfig::Manager::instance();
}
