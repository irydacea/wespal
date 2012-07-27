//
// codename Morning Star
//
// Copyright (C) 2012 by Ignacio Riquelme Morelle <shadowm2006@gmail.com>
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

PaletteItemDelegate::PaletteItemDelegate(QObject *parent) :
	QStyledItemDelegate(parent)
{
}

void PaletteItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	PainterRestorer restorer(painter);

	painter->setClipping(true);

	if(option.state & QStyle::State_Selected) {
		QPen pen(Qt::black, 4);
		painter->setPen(pen);
	}

	QColor color = index.data(Qt::UserRole).toInt();
	painter->setBrush(color);

	const QRect& area = option.rect;


	painter->drawRect(area);
}

bool PaletteItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
	if(event->type() != QEvent::MouseButtonDblClick)
		return false;

	QColor color = index.data(Qt::UserRole).toInt();
	color = QColorDialog::getColor(color);

	if(color.isValid())
		model->setData(index, color.rgb(), Qt::UserRole);

	return true;
}
