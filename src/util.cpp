//
// codename Morning Star
//
// Copyright (C) 2010 - 2012 by Ignacio Riquelme Morelle <shadowm2006@gmail.com>
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

#include "util.hpp"

#include <QIcon>
#include <QMessageBox>

namespace {

/** Namespace wrapper for QObject::tr(). */
inline QString tr(const char* text) { return QObject::tr(text); }

bool doJobNotification(QWidget* parent, const QString& message, const QString& details, QMessageBox::Icon icon)
{
	QMessageBox msg(parent);

	msg.setWindowTitle(tr("Wesnoth RCX"));
	msg.setText(message);
	msg.setDetailedText(details);
	msg.setIcon(icon);
	msg.setWindowModality(Qt::WindowModal);

	if(icon == QMessageBox::Question) {
		msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	}

	const QMessageBox::StandardButton result =
		QMessageBox::StandardButton(msg.exec());

	return icon == QMessageBox::Question
		? result == QMessageBox::Yes
		: true;
}

bool doJobNotification(QWidget* parent, const QString& message, const QStringList& detailItems, QMessageBox::Icon icon)
{
	QString joinedList = detailItems.join("\n");
	return doJobNotification(parent, message, joinedList, icon);
}

} // end anonymous namespace

namespace MosUi {

void message(QWidget* parent, const QString& message, const QString& details)
{
	doJobNotification(parent, message, details, QMessageBox::Information);
}

void message(QWidget* parent, const QString& message, const QStringList& detailItems)
{
	doJobNotification(parent, message, detailItems, QMessageBox::Information);
}

bool prompt(QWidget* parent, const QString& message, const QString& details)
{
	return doJobNotification(parent, message, details, QMessageBox::Question);
}

bool prompt(QWidget* parent, const QString& message, const QStringList& detailItems)
{
	return doJobNotification(parent, message, detailItems, QMessageBox::Question);
}

void error(QWidget* parent, const QString& message, const QString& details)
{
	doJobNotification(parent, message, details, QMessageBox::Critical);
}

void error(QWidget* parent, const QString& message, const QStringList& detailItems)
{
	doJobNotification(parent, message, detailItems, QMessageBox::Critical);
}

} // end namespace JobNotifications
