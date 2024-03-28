//
// codename Morning Star
//
// Copyright (C) 2010 - 2024 by Iris Morelle <iris@irydacea.me>
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

#include <QPointer>
#include <QStringList>
#include <QWidget>

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

namespace MosPlatform {

/**
 * Retrieves a list of supported image file formats.
 *
 * @note The returned value is meant to be used with QFileDialog.
 */
QString supportedImageFileFormats();

} // end namespace MosPlatform

namespace MosUi {

void message(QWidget* parent, const QString& message, const QString& details = "");
void message(QWidget* parent, const QString& message, const QStringList& detailItems);

bool prompt(QWidget* parent, const QString& message, const QString& details = "");
bool prompt(QWidget* parent, const QString& message, const QStringList& detailItems);

void error(QWidget* parent, const QString& message, const QString& details = "");
void error(QWidget* parent, const QString& message, const QStringList& detailItems);

void about(QWidget* parent = nullptr);

} // end namespace JobUi
