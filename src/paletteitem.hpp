//
// codename Morning Star
//
// Copyright (C) 2012 - 2019 by Iris Morelle <shadowm2006@gmail.com>
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

#pragma once

#include <QStyledItemDelegate>

class PaletteItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	explicit PaletteItemDelegate(QObject *parent = nullptr);

	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
		const QSize& base_size = QStyledItemDelegate::sizeHint(option, index);
		return QSize(qMax(width_, base_size.width()), qMax(height_, base_size.height()));
	}
	
signals:
	
public slots:



private:

	const int border_size_, width_, height_;
	
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
QIcon createColorIcon(const QColor &color,
					  const QSize& size,
					  const QWidget* target = nullptr);

/**
 * Creates an icon for a color item for use in lists and dropdown menus.
 *
 * Ideally, you want to make use of @a target to get an icon that's correctly
 * rendered for a widget's display device, especially for mixed display setups
 * that could have different pixel densities. It is up to you to request a new
 * icon if the widget gets moved to a different display.
 *
 * @param color                Color value.
 * @param width                Icon width.
 * @param height               Icon height.
 * @param target               Target widget or window, used for DPI scaling.
 *                             If not specified, the default pixel ratio for
 *                             the app is used instead.
 *
 * @return An icon consisting of the specified solid color with a solid black
 *         outline, padded all around by one transparent pixel.
 */
inline QIcon createColorIcon(const QColor &color,
							 int width = 16,
							 int height = 16,
							 const QWidget* target = nullptr)
{
	return createColorIcon(color, QSize{width, height}, target);
}

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
inline QIcon createColorIcon(const QColor &color,
							 const QWidget* target = nullptr)
{
	return createColorIcon(color, QSize{16, 16}, target);
}
