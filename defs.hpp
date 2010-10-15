#ifndef DEFS_HPP
#define DEFS_HPP

#include "wesnothrc.hpp"

extern QStringList			mos_color_range_names;
extern QList<color_range>	mos_color_ranges;

extern QList<QRgb>			mos_pal_magenta;
extern QList<QRgb>			mos_pal_flag_green;
extern QList<QRgb>			mos_pal_ellipse_red;

extern QString mos_color_range_id_to_name(int n);
extern const color_range& mos_color_range_from_id(int n);

#endif // DEFS_HPP
