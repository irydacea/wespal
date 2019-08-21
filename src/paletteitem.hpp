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

QIcon createColorIcon(const QColor &color, int width = 16, int height = 16);
