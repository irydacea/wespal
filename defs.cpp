#include "defs.hpp"

#include <QStringList>

QStringList			mos_color_range_names;	// extern
QList<color_range>	mos_color_ranges;		// extern

QString mos_color_range_id_to_name(int n)
{
	if(n > mos_color_range_names.size() || n < 1) {
		n = 1;
	}

	return mos_color_range_names.at(n);
}

const color_range& mos_color_range_from_id(int n)
{
	if(n > mos_color_range_names.size() || n < 1) {
		n = 1;
	}

	return mos_color_ranges.at(n);
}

static struct _defs_ctor_ {
	_defs_ctor_()
	{
		// Definitions taken from /data/core/team-colors.cfg

		mos_color_range_names
			<< "red"
			<< "blue"
			<< "green"
			<< "purple"
			<< "black"
			<< "brown"
			<< "orange"
			<< "white"
			<< "teal"
		;
		mos_color_ranges
			<< color_range(0xFF0000, 0xFFFFFF, 0x000000)
			<< color_range(0x2E419B, 0xFFFFFF, 0x0F0F0F)
			<< color_range(0x62B664, 0xFFFFFF, 0x000000)
			<< color_range(0x93009D, 0xFFFFFF, 0x000000)
			<< color_range(0x5A5A5A, 0xFFFFFF, 0x000000)
			<< color_range(0x945027, 0xFFFFFF, 0x000000)
			<< color_range(0xFF7E00, 0xFFFFFF, 0x0F0F0F)
			<< color_range(0xE1E1E1, 0xFFFFFF, 0x1E1E1E)
			<< color_range(0x30CBC0, 0xFFFFFF, 0x000000)
		;
	}
} _ctor;
