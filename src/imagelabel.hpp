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

#ifndef IMAGELABEL_HPP
#define IMAGELABEL_HPP

#include <QWidget>

class ImageLabel : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap)
public:
	explicit ImageLabel(QWidget* parent = 0);
	
	const QPixmap* pixmap() const {
		return &pixmap_;
	}

	void setPixmap(const QPixmap& pixmap) {
		pixmap_ = pixmap;
		update();
	}

signals:
	
public slots:

protected:
	virtual void paintEvent(QPaintEvent* event);

private:
	QPixmap pixmap_;
};

#endif // IMAGELABEL_HPP
