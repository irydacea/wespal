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

#include "appconfig.hpp"

#include <QSettings>
#include <QMessageBox>

namespace {
	const unsigned max_recent_files = 4;
}

void mos_config_load(QMap<QString, color_range> &ranges, QMap<QString, QList<QRgb> > &palettes)
{
	QSettings s;

	const int nranges = s.beginReadArray("color_ranges");

	for(int i = 0; i < nranges; ++i) {
		s.setArrayIndex(i);
		const color_range r(
			s.value("avg").toUInt(),
			s.value("max").toUInt(),
			s.value("min").toUInt());
		ranges.insert(s.value("id").toString(), r);
	}

	s.endArray();

	const int npals = s.beginReadArray("palettes");

	for(int i = 0; i < npals; ++i) {
		s.setArrayIndex(i);

		const QStringList vals = s.value("values").toString().split(",", Qt::SkipEmptyParts);
		QList<QRgb> rgblist;

		for(const QString& v : vals) {
			rgblist.push_back(v.toUInt());
		}

		palettes.insert(s.value("id").toString(), rgblist);
	}

	s.endArray();
}

void mos_config_save(const QMap<QString, color_range> &ranges, const QMap<QString, QList<QRgb> > &palettes)
{
	QSettings s;
	int j;

	s.beginWriteArray("color_ranges");
	j = 0;
	for(QMap<QString, color_range>::const_iterator i = ranges.constBegin();
		i != ranges.constEnd(); ++i, ++j)
	{
		s.setArrayIndex(j);
		s.setValue("id", i.key());
		s.setValue("avg", i.value().mid());
		s.setValue("max", i.value().max());
		s.setValue("min", i.value().min());
	}
	s.endArray();

	s.beginWriteArray("palettes");
	j = 0;
	for(QMap<QString, QList<QRgb> >::const_iterator i = palettes.constBegin();
		i != palettes.constEnd(); ++i, ++j)
	{
		s.setArrayIndex(j);
		s.setValue("id", i.key());

		QString csv;

		for(QRgb v : i.value()) {
			if(!csv.isEmpty())
				csv += ',';
			csv += QString::number(v);
		}

		s.setValue("values", csv);
	}
	s.endArray();
}

unsigned mos_max_recent_files()
{
	return max_recent_files;
}

QStringList mos_recent_files()
{
	return QSettings().value("recent_files").toStringList();
}

void mos_add_recent_file(const QString& filepath)
{
	QSettings s;

	QStringList recent = s.value("recent_files").toStringList();
	recent.removeAll(filepath);

	recent.push_front(filepath);

	while(recent.size() > int(max_recent_files)) {
		recent.pop_back();
	}

	s.setValue("recent_files", recent);
}

void mos_remove_recent_file(const QString& filepath)
{
	QSettings s;

	QStringList recent = s.value("recent_files").toStringList();
	recent.removeAll(filepath);

	s.setValue("recent_files", recent);
}

QString mos_get_preview_background_color_name()
{
	return QSettings().value("preview_background").toString();
}

void mos_set_preview_background_color_name(const QString& colorName)
{
	QSettings().setValue("preview_background", colorName);
}

QSize mos_get_main_window_size()
{
	return QSettings().value("mainwindow_size").toSize();
}

void mos_set_main_window_size(const QSize& size)
{
	QSettings().setValue("mainwindow_size", size);
}
