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

#pragma once

#include <QWidget>

/**
 * Widget used for rendering a single image.
 */
class ImageLabel : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QImage image READ image WRITE setImage)
public:
	/**
	 * Constructor.
	 *
	 * @param parent Sets the parent of this widget.
	 */
	explicit ImageLabel(QWidget* parent = nullptr);

	virtual QSize minimumSizeHint() const override
	{
		return image_.size();
	}
	
	/**
	 * Retrieves the current image.
	 */
	const QImage& image() const
	{
		return image_;
	}

	/**
	 * Sets a new image to display.
	 */
	void setImage(const QImage& image);

	/**
	 * Removes the displayed image.
	 */
	void clear();

signals:
	
public slots:

protected:
	virtual void paintEvent(QPaintEvent* event) override;

private:
	QImage image_;
};
