//
// codename Morning Star
//
// Copyright (C) 2010 by Ignacio R. Morelle <shadowm@wesnoth.org>
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

#ifndef SPECS_HPP
#define SPECS_HPP

#include "wesnothrc.hpp"

#include <QString>

template< typename T >
struct labeled_spec
{
	typedef T  labeled_type;
	typedef T& labeled_ref;
	typedef T* labeled_ptr;

	T			def;	/**< Definition.        */
	QString		id;		/**< Internal id.       */
	QString		name;	/**< Translatable name. */

	/** Default constructor */
	labeled_spec()
		: def(), id(), name()
	{}

	/** List constructor. */
	labeled_spec(const labeled_type& _def, const QString& _id, const QString& _name)
		: def(_def)
		, id(_id), name(_name)
	{}

	/** Copy constructor. */
	labeled_spec(const labeled_spec<labeled_type>& o)
		: def(o.def)
		, id(o.id), name(o.name)
	{}

	/** Assignment operator. */
	labeled_spec& operator=(const labeled_spec& o)
	{
		if(&o != this) {
			def = o.def;
			id = o.id; name = o.name;
		}

		return *this;
	}
};

typedef labeled_spec< color_range > range_spec;
typedef labeled_spec< QList<QRgb> > pal_spec;

#endif // SPECS_HPP
