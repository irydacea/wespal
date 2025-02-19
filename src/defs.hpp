/*
 * Wespal (codename Morning Star) - Wesnoth assets recoloring tool
 *
 * Copyright (C) 2010 - 2025 by Iris Morelle <iris@irydacea.me>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#pragma once

#include "wesnothrc.hpp"

#include <QStringList>

struct MissingBuiltinObjectError : public std::exception
{
};

/**
 * Type used for initialising a built-in object store.
 *
 * Do not use for other purposes.
 */
template<typename T>
struct BuiltinObjectDescriptor
{
	typedef T                  ObjectType;
	typedef QString            ObjectNameType;

	ObjectNameType             name;
	ObjectNameType             translatableName;
	ObjectType                 object;
};

/**
 * Type used for a built-in object store.
 *
 * This is a managed collection of objects, each of which has an associated
 * name/identifier, translatable name, and value (as in the object itself).
 * The objects have a manually-defined order, but may also be looked up
 * quickly from an associative container using the array subscript operator,
 * specifying the name/identifier associated with them.
 */
template<typename T>
class BuiltinObjectStore
{
public:
	/** Type of an underlying singular object. */
	typedef T                  ObjectType;
	/** Type of a lightweight handle to a singular object. */
	typedef T*                 HandleType;
	/** Type of a manually-sorted list of lightweight handles to singular objects. */
	typedef QList<HandleType>  OrderedHandleListType;

	/** Type of a singular object name/identifier. */
	typedef QString            ObjectNameType;
	/** Type of a singular translatable object label for UI display. */
	typedef QStringList        OrderedNameListType;

	/** Type of the underlying object store. */
	typedef QMap<QString, ObjectType> StoreType;

	/** Type of the descriptor type used during construction. */
	typedef BuiltinObjectDescriptor<ObjectType> DescriptorType;

	/**
	 * Constructor. This is the only constructor we use.
	 */
	BuiltinObjectStore(std::initializer_list<DescriptorType> initList)
		: store_()
		, orderedNames_()
		, orderedTranslatableNames_()
		, orderedHandles_()
	{
		for (auto& descriptor : initList)
		{
			store_.insert(descriptor.name, descriptor.object);
			orderedNames_.push_back(descriptor.name);
			orderedTranslatableNames_.push_back(descriptor.translatableName);
			orderedHandles_.emplaceBack(&(store_.find(descriptor.name).value()));
		}
	}

	// We only need one instance of these objects ever, so no other
	// constructors for anyone else.

	BuiltinObjectStore() = delete;
	BuiltinObjectStore(const BuiltinObjectStore&) = delete;
	BuiltinObjectStore(BuiltinObjectStore&&) = delete;

	/**
	 * Retrieves a named object from the store.
	 *
	 * @param name The name of the object.
	 *
	 * @return A read-only reference to the object.
	 *
	 * @note This will throw MissingBuiltinObjectError if the object cannot be
	 *       found.
	 */
	const ObjectType& operator[](const ObjectNameType& name) const
	{
		auto it = store_.constFind(name);
		if (it == store_.constEnd()) {
			throw MissingBuiltinObjectError();
		}
		return it.value();
	}

	/**
	 * Returns whether an object with the given name exists.
	 */
	bool hasName(const ObjectNameType& name) const
	{
		auto it = store_.constFind(name);
		return it != store_.constEnd();
	}

	/**
	 * Provides direct read-only access to the underlying store.
	 *
	 * This is meant to be used for quickly merging into other data
	 * structures and nothing else.
	 */
	const StoreType& objects() const
	{
		return store_;
	}

	/**
	 * Returns the stored object count.
	 */
	qsizetype objectCount() const
	{
		return orderedNames_.count();
	}

	/**
	 * Provides a list of object names in their predefined order.
	 */
	const OrderedNameListType& orderedNames() const
	{
		return orderedNames_;
	}

	/**
	 * Provides a list of translatable object names in their predefined order.
	 */
	const OrderedNameListType& orderedTranslatableNames() const
	{
		return orderedTranslatableNames_;
	}

	/**
	 * Provides a list of handles to objects in their predefined order.
	 */
	const OrderedHandleListType& orderedObjects() const
	{
		return orderedHandles_;
	}

private:
	StoreType                 store_;
	OrderedNameListType       orderedNames_;
	OrderedNameListType       orderedTranslatableNames_;
	OrderedHandleListType     orderedHandles_;
};

// Typedefs for simplicity

using BuiltinColorRanges = BuiltinObjectStore<ColorRange>;

using BuiltinPalettes = BuiltinObjectStore<ColorList>;

//
// Definitions of built-in (Wesnoth mainline)
// color ranges and palettes.
//

namespace wesnoth {

/**
 * Built-in color ranges.
 */
extern const BuiltinColorRanges builtinColorRanges;

/**
 * Built-in color palettes.
 */
extern const BuiltinPalettes builtinPalettes;

} // end namespace wesnoth
