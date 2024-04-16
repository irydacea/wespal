/*
 * Wespal (codename Morning Star) - Wesnoth assets recoloring tool
 *
 * Copyright (C) 2012 - 2024 by Iris Morelle <iris@irydacea.me>
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

#include "wesnothrc.hpp"

#include <QStyledItemDelegate>

constexpr QSize defaultColorIconSize{16, 16};

class PaletteItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	explicit PaletteItemDelegate(QObject* parent = nullptr)
		: QStyledItemDelegate(parent)
	{
	}

	virtual void paint(QPainter* painter,
					   const QStyleOptionViewItem& option,
					   const QModelIndex& index) const override;

	virtual bool editorEvent(QEvent* event,
							 QAbstractItemModel* model,
							 const QStyleOptionViewItem& option,
							 const QModelIndex& index) override;

	virtual QSize sizeHint(const QStyleOptionViewItem& option,
						   const QModelIndex& index) const override
	{
		const auto& baseSize = QStyledItemDelegate::sizeHint(option, index);
		return {qMax(width_, baseSize.width()), qMax(height_, baseSize.height())};
	}

signals:
	
public slots:

private:
	static constexpr int borderSize_ = 2;
	static constexpr int width_ = 20;
	static constexpr int height_ = 20;
};

/**
 * Creates an icon for a color item for use in lists and dropdown menus.
 *
 * Ideally, you want to make use of @a target to get an icon that's correctly
 * rendered for a widget's display device, especially for mixed display setups
 * that could have different pixel densities. It is up to you to request a new
 * icon if the widget gets moved to a different display.
 *
 * @param color                Color value.
 * @param size                 Icon size.
 * @param target               Target widget or window, used for DPI scaling.
 *                             If not specified, the default pixel ratio for
 *                             the app is used instead.
 *
 * @return An icon consisting of the specified solid color with a solid black
 *         outline, padded all around by one transparent pixel.
 */
QIcon createColorIcon(const QColor& color,
					  const QSize& size = defaultColorIconSize,
					  const QWidget* target = nullptr);

/**
 * Creates a 16x16 icon for a color item for use in lists and dropdown menus.
 *
 * Ideally, you want to make use of @a target to get an icon that's correctly
 * rendered for a widget's display device, especially for mixed display setups
 * that could have different pixel densities. It is up to you to request a new
 * icon if the widget gets moved to a different display.
 *
 * @param color                Color value.
 * @param target               Target widget or window, used for DPI scaling.
 *                             If not specified, the default pixel ratio for
 *                             the app is used instead.
 *
 * @return An icon consisting of the specified solid color with a solid black
 *         outline, padded all around by one transparent pixel.
 */
inline QIcon createColorIcon(const QColor& color,
							 const QWidget* target = nullptr)
{
	return createColorIcon(color, defaultColorIconSize, target);
}

/**
 * Creates an empty color icon for use in lists and dropdown menus.
 *
 * Ideally, you want to make use of @a target to get an icon that's correctly
 * rendered for a widget's display device, especially for mixed display setups
 * that could have different pixel densities. It is up to you to request a new
 * icon if the widget gets moved to a different display.
 *
 * @param size                 Icon size.
 * @param target               Target widget or window, used for DPI scaling.
 *                             If not specified, the default pixel ratio for
 *                             the app is used instead.
 *
 * @return An icon consisting of a transparent rectangle with a black outline
 *         and a black diagonal line, padded all around by one transparent
 *         pixel.
 */
QIcon createEmptyColorIcon(const QSize& size = defaultColorIconSize,
						   const QWidget* target = nullptr);

/**
 * Creates an empty 16x16 color icon for use in lists and dropdown menus.
 *
 * Ideally, you want to make use of @a target to get an icon that's correctly
 * rendered for a widget's display device, especially for mixed display setups
 * that could have different pixel densities. It is up to you to request a new
 * icon if the widget gets moved to a different display.
 *
 * @param target               Target widget or window, used for DPI scaling.
 *                             If not specified, the default pixel ratio for
 *                             the app is used instead.
 *
 * @return An icon consisting of a transparent rectangle with a black outline
 *         and a black diagonal line, padded all around by one transparent
 *         pixel.
 */
inline QIcon createEmptyColorIcon(const QWidget* target = nullptr)
{
	return createEmptyColorIcon(defaultColorIconSize, target);
}

/**
 * Creates a palette icon for use in lists and dropdown menus.
 *
 * Ideally, you want to make use of @a target to get an icon that's correctly
 * rendered for a widget's display device, especially for mixed display setups
 * that could have different pixel densities. It is up to you to request a new
 * icon if the widget gets moved to a different display.
 *
 * @param palette              Color palette.
 * @param size                 Icon size.
 * @param target               Target widget or window, used for DPI scaling.
 *                             If not specified, the default pixel ratio for
 *                             the app is used instead.
 *
 * @return An icon consisting of the palette's first color with a solid black
 *         outline, padded all around by one transparent pixel. If the palette
 *         is empty, the rectangle will contain a black diagonal line instead.
 */
inline QIcon createPaletteIcon(const ColorList& palette,
							   const QSize& size = defaultColorIconSize,
							   const QWidget* target = nullptr)
{
	if (palette.empty()) {
		return createEmptyColorIcon(size, target);
	} else {
		return createColorIcon(palette.front(), size, target);
	}
}

/**
 * Creates a 16x16 palette icon for use in lists and dropdown menus.
 *
 * Ideally, you want to make use of @a target to get an icon that's correctly
 * rendered for a widget's display device, especially for mixed display setups
 * that could have different pixel densities. It is up to you to request a new
 * icon if the widget gets moved to a different display.
 *
 * @param palette              Color palette.
 * @param target               Target widget or window, used for DPI scaling.
 *                             If not specified, the default pixel ratio for
 *                             the app is used instead.
 *
 * @return An icon consisting of the palette's first color with a solid black
 *         outline, padded all around by one transparent pixel. If the palette
 *         is empty, the rectangle will contain a black diagonal line instead.
 */
inline QIcon createPaletteIcon(const ColorList& palette,
							   const QWidget* target = nullptr)
{
	return createPaletteIcon(palette, defaultColorIconSize, target);
}

/**
 * Creates a color range icon for use in lists and dropdown menus.
 *
 * Ideally, you want to make use of @a target to get an icon that's correctly
 * rendered for a widget's display device, especially for mixed display setups
 * that could have different pixel densities. It is up to you to request a new
 * icon if the widget gets moved to a different display.
 *
 * @param color                Color value.
 * @param size                 Icon size.
 * @param target               Target widget or window, used for DPI scaling.
 *                             If not specified, the default pixel ratio for
 *                             the app is used instead.
 *
 * @return An icon consisting of the color range's marker color with a solid
 *         black outline, padded all around by one transparent pixel.
 */
inline QIcon createColorRangeIcon(const ColorRange& colorRange,
								  const QSize& size = defaultColorIconSize,
								  const QWidget* target = nullptr)
{
	return createColorIcon(colorRange.rep(), size, target);
}

/**
 * Creates a 16x16 color range icon for use in lists and dropdown menus.
 *
 * Ideally, you want to make use of @a target to get an icon that's correctly
 * rendered for a widget's display device, especially for mixed display setups
 * that could have different pixel densities. It is up to you to request a new
 * icon if the widget gets moved to a different display.
 *
 * @param color                Color value.
 * @param target               Target widget or window, used for DPI scaling.
 *                             If not specified, the default pixel ratio for
 *                             the app is used instead.
 *
 * @return An icon consisting of the color range's marker color with a solid
 *         black outline, padded all around by one transparent pixel.
 */
inline QIcon createColorRangeIcon(const ColorRange& colorRange,
								  const QWidget* target = nullptr)
{
	return createColorRangeIcon(colorRange, defaultColorIconSize, target);
}
