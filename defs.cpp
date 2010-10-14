#include "defs.hpp"
#if 0
color_range_map builtin_color_ranges;

color_range_tag builtin_cr_red(1, "Red", 0, 0, 0);
color_range_tag builtin_cr_red(2, "Red", 0, 0, 0);
color_range_tag builtin_cr_red(3, "Red", 0, 0, 0);
color_range_tag builtin_cr_red(4, "Red", 0, 0, 0);
color_range_tag builtin_cr_red(5, "Red", 0, 0, 0);
color_range_tag builtin_cr_red(6, "Red", 0, 0, 0);
color_range_tag builtin_cr_red(7, "Red", 0, 0, 0);
color_range_tag builtin_cr_red(8, "Red", 0, 0, 0);
color_range_tag builtin_cr_red(9, "Red", 0, 0, 0);

static struct defs_init_ctl {
	defs_init_ctl()
	{
		builtin_color_ranges["red"] =
	}
} dummy;


color_range_tag::color_range_tag(int num, QString tname, unsigned avg, unsigned hi, unsigned low)
	: team_num(num)
	, name(tname)
	, object(avg, hi, low)
{
}
#endif
