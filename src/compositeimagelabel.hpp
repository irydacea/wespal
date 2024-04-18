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

#pragma once

#include <QScrollArea>

class QPainter;
class QRectF;

/**
 * Widget used for rendering an image with a "left side" and a "right side".
 *
 * This widget supports two rendering modes:
 *
 *   1. One portion of the widget is the "left side", while the other is the
 *      "right side". A property can specify the left:right width ratio.
 *
 *   2. The "right side" is rendered superimposed on the "left side" (a.k.a.
 *      onion skin view).
 *
 * Both pixmaps are assumed to have the same dimensions. Expect weird things
 * to happen if this is not true, especially if the aspect ratios differ!
 */
class CompositeImageLabel : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(CompositeDisplayMode displayMode READ displayMode WRITE setDisplayMode)

	Q_PROPERTY(qreal displayRatio READ displayRatio WRITE setDisplayRatio)

	Q_PROPERTY(QImage imageLeft READ leftImage WRITE setLeftImage)
	Q_PROPERTY(QImage imageRight READ rightImage WRITE setRightImage)

public:
	/**
	 * Constructor.
	 *
	 * @param parent Sets the parent of this widget.
	 */
	explicit CompositeImageLabel(QWidget* parent = nullptr);

	virtual QSize minimumSizeHint() const override
	{
		return leftImage_.size();
	}

	/**
	 * These are the possible display modes for the two images.
	 */
	enum CompositeDisplayMode
	{
		/** Allows the user to swipe left and right between images. */
		CompositeDisplaySliding,
		/** Reveals the left image gradually. */
		CompositeDisplayOnionSkin,
	};

	Q_ENUM(CompositeDisplayMode);

	/**
	 * Retrieves the current composite image display mode.
	 */
	CompositeDisplayMode displayMode() const
	{
		return displayMode_;
	}

	/**
	 * Sets the composite image display mode.
	 */
	void setDisplayMode(CompositeDisplayMode newMode)
	{
		displayMode_ = newMode;
		buildComposite();
		update();
	}

	/**
	 * Retrieves the current left:right image display ratio.
	 *
	 * Values are in the range [0.0, 1.0] (inclusive). A value of 0.0 means
	 * only the right image is displayed, while a value of 1.0 means only the
	 * left image is displayed.
	 */
	qreal displayRatio() const
	{
		return displayRatio_;
	}

	/**
	 * Sets the left:right image display ratio.
	 *
	 * The new value is expected to be in the range [0.0, 1.0] (inclusive). A
	 * value of 0.0 means only the right image is displayed, while a value of
	 * 1.0 means only the left image is displayed.
	 */
	void setDisplayRatio(qreal displayRatio)
	{
		if (displayRatio == displayRatio_)
			return;

		displayRatio_ = qBound(0.0, displayRatio, 1.0);
		buildComposite();
		update();
	}

	/**
	 * Retrieves the left pixmap.
	 */
	const QImage& leftImage() const
	{
		return leftImage_;
	}

	/**
	 * Sets the left image to the specified QImage.
	 */
	void setLeftImage(const QImage& leftImage);

	/**
	 * Retrieves the right image.
	 */
	const QImage& rightImage() const
	{
		return rightImage_;
	}

	/**
	 * Sets the right pixmap to the specified QImage.
	 */
	void setRightImage(const QImage& rightImage);

	/**
	 * Sets the left and right images simultaneously.
	 */
	void setImages(const QImage& leftImage, const QImage& rightImage);

	/**
	 * Removes the left and right images.
	 */
	void clear();

signals:

public slots:

protected:
	virtual void paintEvent(QPaintEvent* event) override;

private:
	void buildComposite();

	CompositeDisplayMode displayMode_;

	qreal displayRatio_;

	QImage leftImage_;
	QImage rightImage_;

	QImage compositeCache_;
};
