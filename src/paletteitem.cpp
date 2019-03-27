//
// codename Morning Star
//
// Copyright (C) 2012 - 2018 by Iris Morelle <shadowm2006@gmail.com>
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

#include "paletteitem.hpp"

#include <QColorDialog>
#include <QEvent>
#include <QPainter>

namespace {
	struct PainterRestorer
	{
		PainterRestorer(QPainter *painter) : painter_(painter) {
			painter_->save();
		}
		~PainterRestorer() {
			painter_->restore();
		}
	private:
		QPainter *painter_;
	};
}

QIcon createColorIcon(const QColor& color, int width, int height)
{
	QPixmap base(width, height);
	base.fill(QColor(255, 255, 255, 0));

	QPainter painter(&base);
	QBrush brush(color);
	QPen pen(Qt::black, 1);

	painter.setBrush(brush);
	painter.setPen(pen);
	painter.drawRect(base.rect().adjusted(1,1,-2,-2));

	return base;
}

PaletteItemDelegate::PaletteItemDelegate(QObject *parent) :
	QStyledItemDelegate(parent),
	border_size_(2),
	width_(16), height_(17)
{
}

void PaletteItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	PainterRestorer restorer(painter);

	// The actual item region.
	QRect area = option.rect;

	QPen pen;

	if(option.state & QStyle::State_Selected) {
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
	if(event->type() != QEvent::MouseButtonDblClick)
		return false;

	QColor color = index.data(Qt::UserRole).toUInt();
	color = QColorDialog::getColor(color, qobject_cast<QWidget*>(this->parent()));

	if(color.isValid())
		model->setData(index, color.rgb(), Qt::UserRole);

	return true;
}
