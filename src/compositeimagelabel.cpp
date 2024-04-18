/*
 * Wespal (codename Morning Star) - Wesnoth assets recoloring tool
 *
 * Copyright (C) 2024 by Iris Morelle <iris@irydacea.me>
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

#include "compositeimagelabel.hpp"

#include <QLayout>
#include <QPainter>
#include <QPaintEvent>
#include <QScrollBar>

CompositeImageLabel::CompositeImageLabel(QWidget* parent)
	: QWidget(parent)
	, displayMode_(CompositeDisplaySliding)
	, displayRatio_(0.5)
	, leftImage_()
	, rightImage_()
{
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

void CompositeImageLabel::setLeftImage(const QImage& leftImage)
{
	leftImage_ = leftImage.convertedTo(QImage::Format_ARGB32_Premultiplied);

	buildComposite();
	updateGeometry();
	update();
}

void CompositeImageLabel::setRightImage(const QImage& rightImage)
{
	rightImage_ = rightImage.convertedTo(QImage::Format_ARGB32_Premultiplied);

	buildComposite();
	updateGeometry();
	update();
}

void CompositeImageLabel::setImages(const QImage& leftImage, const QImage& rightImage)
{
	leftImage_ = leftImage.convertedTo(QImage::Format_ARGB32_Premultiplied);
	rightImage_ = rightImage.convertedTo(QImage::Format_ARGB32_Premultiplied);

	buildComposite();
	updateGeometry();
	update();
}

void CompositeImageLabel::clear()
{
	compositeCache_ = rightImage_ = leftImage_ = QImage{};

	buildComposite();
	updateGeometry();
	update();
}

void CompositeImageLabel::buildComposite()
{
	auto& left = leftImage_;
	auto& right = rightImage_;

	// Optimise the best case scenarios by referencing a single image.

	if (displayRatio_ == 0.0) {
		compositeCache_ = left;
		return;
	} else if (displayRatio_ == 1.0) {
		compositeCache_ = right;
		return;
	}

	QImage::Format fmt = displayMode_ == CompositeDisplayOnionSkin
						 ? QImage::Format_ARGB32
						 : QImage::Format_ARGB32_Premultiplied;

	QImage compositeRender{leftImage_.size(), fmt};
	compositeRender.fill(0);
	QPainter o{&compositeRender};

	switch (displayMode_)
	{
		case CompositeDisplaySliding: {
			QRect rightRect{
				0,
				0,
				qRound(displayRatio_ * rightImage_.width()),
				rightImage_.height()
			};
			QRect leftRect{rightRect};

			leftRect.setWidth(rightImage_.width() - rightRect.width());
			leftRect.translate(rightRect.width(), 0);

			o.drawImage(rightRect, right, rightRect);
			o.drawImage(leftRect, left, leftRect);

			break;
		}
		case CompositeDisplayOnionSkin: {
			auto maxY = qMin(left.height(), right.height()),
				 maxX = qMin(left.width(), right.width());

			for (int y = 0; y < maxY; ++y)
			{
				const auto* leftLine = reinterpret_cast<const QRgb*>(left.constScanLine(y));
				const auto* rightLine = reinterpret_cast<const QRgb*>(right.constScanLine(y));
				auto* compositeLine = reinterpret_cast<QRgb*>(compositeRender.scanLine(y));

				for (int x = 0; x < maxX; ++x)
				{
					// Manual alpha blending wheeeeee

					auto leftCol = qUnpremultiply(leftLine[x]) & 0xFFFFFFU;
					auto rightCol = qUnpremultiply(rightLine[x]) & 0xFFFFFFU;

					auto r = displayRatio_ * qRed(rightCol) + (1 - displayRatio_) * qRed(leftCol);
					auto g = displayRatio_ * qGreen(rightCol) + (1 - displayRatio_) * qGreen(leftCol);
					auto b = displayRatio_ * qBlue(rightCol) + (1 - displayRatio_) * qBlue(leftCol);

					compositeLine[x] = qRgba(r, g, b, qAlpha(leftLine[x]));
				}
			}

			break;
		}
		default:
			// Shouldn't be here
			Q_ASSERT(false);
	}

	compositeCache_ = compositeRender;
}

void CompositeImageLabel::paintEvent(QPaintEvent* event)
{
	if (leftImage_.isNull())
		return;

	QPainter p{this};

	p.setClipRect(event->rect());
	p.setRenderHint(QPainter::SmoothPixmapTransform, false);
	p.drawImage(rect(), compositeCache_);
}
