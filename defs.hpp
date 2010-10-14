#ifndef DEFS_HPP
#define DEFS_HPP

#include "wesnothrc.hpp"

#include <map>
#include <qstring.h>

struct color_range_tag
{
	QString name;
	int team_num;

	color_range object;

	color_range_tag(int num, QString tname, unsigned avg, unsigned hi, unsigned low);
};

typedef std::map<QString, color_range_tag> color_range_map;
color_range_map color_ranges;


#endif // DEFS_HPP
