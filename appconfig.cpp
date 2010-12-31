//
// codename Morning Star
//
// Copyright (C) 2010, 2011 by Ignacio R. Morelle <shadowm@wesnoth.org>
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

void mos_config_load(QList<range_spec>& ranges, QList<pal_spec>& palettes)
{
	QSettings s;

	const int nranges = s.beginReadArray("color_ranges");

	for(int i = 0; i < nranges; ++i) {
		s.setArrayIndex(i);
		const color_range r(
			s.value("avg").toUInt(),
			s.value("max").toUInt(),
			s.value("min").toUInt());
		ranges.push_back(range_spec(
			r, s.value("id").toString(), s.value("name").toString()
		));
	}

	s.endArray();

	const int npals = s.beginReadArray("palettes");

	for(int i = 0; i < npals; ++i) {
		s.setArrayIndex(i);

		const QStringList vals = s.value("values").toStringList();
		QList<QRgb> rgblist;

		foreach(const QString& v, vals) {
			rgblist.push_back(v.toUInt());
		}

		palettes.push_back(pal_spec(
			rgblist, s.value("id").toString(), s.value("name").toString()
		));
	}

	s.endArray();
}

void mos_config_save(const QList<range_spec>& ranges, const QList<pal_spec>& palettes)
{
	QSettings s;

	s.beginWriteArray("color_ranges");
	for(int i = 0; i < ranges.size(); ++i) {
		s.setArrayIndex(i);
		s.setValue("id", ranges.at(i).id);
		s.setValue("name", ranges.at(i).name);
		// Definition
		s.setValue("avg", ranges.at(i).def.mid());
		s.setValue("max", ranges.at(i).def.max());
		s.setValue("min", ranges.at(i).def.min());
	}
	s.endArray();

	s.beginWriteArray("palettes");
	for(int i = 0; i < palettes.size(); ++i) {
		s.setArrayIndex(i);
		s.setValue("id", palettes.at(i).id);
		s.setValue("name", palettes.at(i).name);

		QString csv;

		foreach(QRgb v, palettes.at(i).def) {
			if(!csv.isEmpty())
				csv += ',';
			csv += QString::number(v);
		}

		s.setValue("values", csv);
	}

	s.endArray();

}
