//
// codename Morning Star
//
// Copyright (C) 2010 - 2019 by Iris Morelle <shadowm2006@gmail.com>
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

#ifndef UTIL_HPP
#define UTIL_HPP

#include <QPointer>
#include <QStringList>
#include <QWidget>

class ObjectLock
{
public:
	ObjectLock(QObject* o)
		: o_(o)
		, initial_state_(o ? o->blockSignals(true) : bool())
	{
	}

	~ObjectLock()
	{
		if(o_) {
			o_->blockSignals(initial_state_);
		}
	}

private:
	QPointer<QObject> o_;
	bool initial_state_;
};

QString capitalize(const QString& str);

namespace MosUi {

void message(QWidget* parent, const QString& message, const QString& details = "");
void message(QWidget* parent, const QString& message, const QStringList& detailItems);

bool prompt(QWidget* parent, const QString& message, const QString& details = "");
bool prompt(QWidget* parent, const QString& message, const QStringList& detailItems);

void error(QWidget* parent, const QString& message, const QString& details = "");
void error(QWidget* parent, const QString& message, const QStringList& detailItems);

void about(QWidget* parent = NULL);

} // end namespace JobUi

#endif // UTIL_HPP
