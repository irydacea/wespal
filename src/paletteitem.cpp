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

#include "paletteitem.hpp"

#include <QGuiApplication>
#include <QColorDialog>
#include <QEvent>
#include <QPainter>

namespace {

struct PainterRestorer
{
	PainterRestorer(QPainter* painter)
		: painter_(painter)
	{
		painter_->save();
	}

	~PainterRestorer()
	{
		painter_->restore();
	}

private:
	QPainter *painter_;
};

inline qreal pixelRatioFallback(const QWidget* target)
{
	return target ? target->devicePixelRatio() : qGuiApp->devicePixelRatio();
}

} // end unnamed namespace

QIcon createColorIconPrivate(const QColor& color,
							 const QSize& size,
							 const QWidget* target,
							 bool drawSlash)
{
	static constexpr qreal borderWidth = 1.0;
	static constexpr qreal outerMargin = 2.0;
	static constexpr QMarginsF innerMargin = {
		outerMargin, outerMargin,
		outerMargin, outerMargin
	};

	const auto pixelRatio = pixelRatioFallback(target);

	QPixmap base{size * pixelRatio};

	base.setDevicePixelRatio(pixelRatio);
	base.fill(Qt::transparent);

	QPainter painter{&base};
	QBrush brush{color};
	QPen pen{Qt::black, borderWidth};
	QRectF borderRect{QPointF{outerMargin, outerMargin},
					  QSizeF{size}.shrunkBy(innerMargin)};

	painter.setBrush(brush);
	painter.setPen(pen);

	if (drawSlash) {
		QLineF slash{borderRect.topRight(), borderRect.bottomLeft()};
		painter.drawLine(slash);
	}

	painter.drawRect(borderRect);

	return base;
}

QIcon createColorIcon(const QColor& color,
					  const QSize& size,
					  const QWidget* target)
{
	return createColorIconPrivate(color, size, target, false);
}

QIcon createEmptyColorIcon(const QSize& size,
						   const QWidget* target)
{
	return createColorIconPrivate(Qt::transparent, size, target, true);
}

void PaletteItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	PainterRestorer restorer{painter};

	// The actual item region.
	QRect area{option.rect};

	QPen pen;

	if (option.state & QStyle::State_Selected) {
		pen.setStyle(Qt::SolidLine);
		pen.setCapStyle(Qt::SquareCap);
		pen.setJoinStyle(Qt::MiterJoin);
		pen.setWidth(border_size_);
		pen.setColor(option.palette.color(QPalette::Highlight));
		// Shrink the rectangle so the borders can fit.
		area.adjust(border_size_/2, border_size_/2, -border_size_/2, -border_size_/2);
	} else {
		pen.setStyle(Qt::NoPen);
	}

	painter->setPen(pen);
	painter->setBrush(QColor{index.data(Qt::UserRole).toUInt()});
	painter->drawRect(area);
}

bool PaletteItemDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& /*option*/, const QModelIndex& index)
{
	if (event->type() != QEvent::MouseButtonDblClick)
		return false;

	QColor color = index.data(Qt::UserRole).toUInt();
	color = QColorDialog::getColor(color, qobject_cast<QWidget*>(this->parent()));

	if (color.isValid())
		model->setData(index, color.rgb(), Qt::UserRole);

	return true;
}
