/*
 * Wespal (codename Morning Star) - Wesnoth assets recoloring tool
 *
 * Copyright (C) 2010 - 2025 by Iris Morelle <iris@irydacea.me>
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

#include <QPointer>
#include <QWidget>

class QAbstractSlider;

/**
 * Helper class used to block signals from objects for a certain scope.
 *
 * @note The object's original blockSignals() value is restored once the
 *       ObjectLock is destroyed, whether it was true or false.
 */
class ObjectLock
{
public:
	ObjectLock(QObject* o)
		: o_(o)
		, initial_state_(o ? o->blockSignals(true) : bool())
	{
	}

	virtual ~ObjectLock()
	{
		if(o_) {
			o_->blockSignals(initial_state_);
		}
	}

private:
	QPointer<QObject> o_;
	bool initial_state_;
};

/**
 * Helper class used to block signals from objects for a certain scope.
 *
 * Unlike ObjectLock, this only has an effect if the specified condition is
 * true at construction time.
 */
class ConditionalObjectLock : public ObjectLock
{
public:
	ConditionalObjectLock(QObject* o, bool condition)
		: ObjectLock(condition ? o : nullptr)
	{
	}

	virtual ~ConditionalObjectLock()
	{
	}
};

/**
 * Used to set a cursor for the duration of scope.
 */
class ScopedCursor
{
public:
	ScopedCursor(QWidget& target,
				 const QCursor& newCursor)
		: target_(target)
	{
		target.setCursor(newCursor);
	}

	~ScopedCursor()
	{
		target_.unsetCursor();
	}

private:
	QWidget& target_;
};

/**
 * Capitalizes the first character in the provided string.
 *
 * @param str          Input string
 *
 * @return A capitalized string.
 */
inline QString capitalize(const QString& str)
{
	QString res{str};

	if(!res.isEmpty()) {
		res.front() = res.front().toUpper();
	}

	return res;
}

QString cleanFileName(const QString& fileName);

namespace MosPlatform {

/**
 * Retrieves a list of supported image file formats.
 *
 * @note The returned value is meant to be used with QFileDialog.
 */
QString supportedImageFileFormats();

/**
 * Retrieves the path to the desktop environment's Pictures folder.
 */
QString desktopPicturesFolderPath();

} // end namespace MosPlatform

namespace MosUi {

void message(QWidget* parent, const QString& message, const QString& details = "");
void message(QWidget* parent, const QString& message, const QStringList& detailItems);

bool prompt(QWidget* parent, const QString& message, const QString& details = "");
bool prompt(QWidget* parent, const QString& message, const QStringList& detailItems);

void error(QWidget* parent, const QString& message, const QString& details = "");
void error(QWidget* parent, const QString& message, const QStringList& detailItems);

void about(QWidget* parent = nullptr);

void openReleaseNotes();

void openIssueTracker();

/**
 * Displays text as a tooltip underneath/beside a slider's grabbable portion.
 *
 * @param slider   A slider widget under which the tooltip will be shown.
 *
 * @param newValue The new value of the slider. This is required because if
 *                 the function is called from the sliderMoved signal, the
 *                 slider's own value() will be outdated, resulting in an
 *                 inaccurate tooltip text.
 *
 * @param text     The tooltip text that will be shown. If this is an empty
 *                 string, the value of the slider as a percentage (compared
 *                 to its minimum and maximum range) will be shown instead.
 */
void displaySliderTextToolTip(QAbstractSlider* slider, int newValue, const QString& text = {});

} // end namespace JobUi
