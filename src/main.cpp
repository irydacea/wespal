//
// codename Morning Star
//
// Copyright (C) 2008 - 2019 by Iris Morelle <shadowm2006@gmail.com>
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

#include "mainwindow.hpp"
#include "version.hpp"

#include <QApplication>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	QString initial_file = "";
	QStringList argvq = a.arguments();

	if(argvq.count() > 1)
		initial_file = argvq.last();

	QCoreApplication::setApplicationName("Morning Star");
	QCoreApplication::setOrganizationName("Wesnoth");
	QCoreApplication::setOrganizationDomain("wesnoth.org");
	QCoreApplication::setApplicationVersion(mos_version);

#ifdef Q_OS_MACOS
	QCoreApplication::setAttribute(Qt::AA_DontShowIconsInMenus);
#endif

	a.setWindowIcon(QIcon(":/wesnoth-rcx-icon-64.png"));

	MainWindow w;
	w.show();

	QTimer::singleShot(0, [&w, &initial_file]() {
		if(!w.initial_open(initial_file)) {
			QApplication::exit(1);
		}
	});

	return a.exec();
}
