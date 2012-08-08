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

#include "imagelabel.hpp"

#include <qmath.h>
#include <QPainter>
#include <QPaintEvent>

ImageLabel::ImageLabel(QWidget *parent) :
	QWidget(parent),
	pixmap_()
{
}

void ImageLabel::paintEvent(QPaintEvent* event)
{
	// We only paint the pixmap in this subclass.
	QPixmap const * const pm = pixmap();

	if(!pm) {
		return;
	}

	QPainter p(this);
	p.setRenderHint(QPainter::SmoothPixmapTransform, false);

	const qreal xRatio = qreal(width()) / qreal(pm->width());
	const qreal yRatio = qreal(height()) / qreal(pm->height());

	QRect partialDestRect = event->rect();

	p.setClipRect(partialDestRect);

	// We must maintain a fixed aspect ratio when scaling up,
	// otherwise pixels cease to be squares.

	const int intXRatio = qCeil(xRatio);
	if(partialDestRect.width() % intXRatio != 0) {
		partialDestRect.setWidth((partialDestRect.width()/intXRatio + 1) * intXRatio);
	}

	const int intYRatio = qCeil(yRatio);
	if(partialDestRect.height() % intYRatio != 0) {
		partialDestRect.setHeight((partialDestRect.height()/intYRatio + 1) * intYRatio);
	}

	QRect partialSrcRect(
		partialDestRect.x() / xRatio,
		partialDestRect.y() / yRatio,
		partialDestRect.width() / xRatio,
		partialDestRect.height() / yRatio);

	//qDebug() << partialSrcRect << " --> " << partialDestRect << " (scale " << xRatio << "x" << yRatio << ")";

	p.drawPixmap(partialDestRect, *pm, partialSrcRect);
}
