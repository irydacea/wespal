//
// wesnoth-tc - Wesnoth Team-Colorizer
// src/util.hpp - Utility defines and methods
//
// Copyright (C) 2003-2008 by the Battle for Wesnoth Project <www.wesnoth.org>
// Copyright (C) 2008, 2009 by Ignacio R. Morelle <shadowm2006@gmail.com>
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

//! @file util.hpp
//! Templates and utility-routines for strings and numbers.

#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <sstream>

#include <boost/cstdint.hpp>

struct bad_lexical_cast {};

template<typename To, typename From>
To lexical_cast(From a)
{
	To res;
	std::stringstream str;

	if(!(str << a && str >> res)) {
		throw bad_lexical_cast();
	} else {
		return res;
	}
}

template<typename To, typename From>
To lexical_cast_default(From a, To def=To())
{
	To res;
	std::stringstream str;

	if(!(str << a && str >> res)) {
		return def;
	} else {
		return res;
	}
}

template<>
int lexical_cast<int, const std::string&>(const std::string& a);

template<>
int lexical_cast<int, const char*>(const char* a);

template<>
int lexical_cast_default<int, const std::string&>(const std::string& a, int def);

template<>
int lexical_cast_default<int, const char*>(const char* a, int def);

template<typename To, typename From>
To lexical_cast_in_range(From a, To def, To min, To max)
{
	To res;
	std::stringstream str;

	if(!(str << a && str >> res)) {
		return def;
	} else {
		if(res < min) {
			return min;
		}
		if(res > max) {
			return max;
		}
		return res;
	}
}

#endif

// kate: indent-mode normal; encoding utf-8; space-indent off; indent-width 4;
