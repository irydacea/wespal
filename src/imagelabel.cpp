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

#include "imagelabel.hpp"

#include <qmath.h>
#include <QPainter>
#include <QPaintEvent>

ImageLabel::ImageLabel(QWidget *parent) :
	QWidget(parent),
	image_()
{
}

void ImageLabel::setImage(const QImage& image)
{
	image_ = image.convertedTo(QImage::Format_ARGB32_Premultiplied);
	update();
}

void ImageLabel::paintEvent(QPaintEvent* event)
{
	if (image_.isNull())
		return;

	QPainter p{this};

	p.setClipRect(event->rect());
	p.setRenderHint(QPainter::SmoothPixmapTransform, false);

	p.drawImage(rect(), image_);
}
