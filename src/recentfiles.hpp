/*
 * Wespal (codename Morning Star) - Wesnoth assets recoloring tool
 *
 * Copyright (C) 2025 by Iris Morelle <iris@irydacea.me>
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

#include <QImage>

namespace MosConfig {

class MruEntry
{
public:
	/**
	 * Constructs an empty MRU entry.
	 */
	MruEntry()
		: filePath_()
		, thumbnail_()
		, miniThumbnail_()
	{
	}

	/**
	 * Constructs a new MRU entry.
	 *
	 * @param filePath         Path to the file.
	 * @param thumbnailData    Thumbnail as base 64 data.
	 */
	MruEntry(const QString& filePath,
			 const QString& thumbnailData);

	/**
	 * Constructs a MRU entry with an automatically-generated thumbnail.
	 *
	 * @param filePath         Path to the file.
	 * @param image            QImage to generate the thumbnail from.
	 *
	 */
	MruEntry(const QString& filePath, const QImage& image);

	/**
	 * Returns whether this MRU entry is empty.
	 */
	bool empty() const
	{
		return filePath_.isEmpty();
	}

	/**
	 * Returns the path to the file.
	 */
	const QString& filePath() const
	{
		return filePath_;
	}

	/**
	 * Returns the current associated thumbnail.
	 */
	const QImage& thumbnail() const
	{
		return thumbnail_;
	}

	/**
	 * Returns the current associated small thumbnail.
	 */
	const QImage& miniThumbnail() const
	{
		return miniThumbnail_;
	}

	/**
	 * Replaces the previously-set thumbnail.
	 */
	void setThumbnail(const QImage& thumbnail)
	{
		thumbnail_ = thumbnail;
	}

	/**
	 * Returns the standard thumbnail size.
	 */
	static const QSize& thumbnailSize();

	/**
	 * Returns the small thumbnail size.
	 */
	static const QSize& miniThumbnailSize();

private:
	MruEntry(const QString& filePath)
		: filePath_(filePath)
		, thumbnail_()
		, miniThumbnail_()
	{
	}

	QString filePath_;
	QImage thumbnail_;
	QImage miniThumbnail_;
};

class MruList
{
public:
	typedef MruEntry&                        ReferenceType;
	typedef const MruEntry&                  ConstReferenceType;
	typedef QList<MruEntry>                  ListType;
	typedef ListType::reverse_iterator       IteratorType;
	typedef ListType::const_reverse_iterator ConstIteratorType;

	MruList(unsigned size = 8)
		: mru_()
		, size_(size)
	{
		mru_.reserve(size + 1);
	}

	template<typename... Args>
	void push(Args&&... args)
	{
		auto entry = MruEntry{args...};

		// Allowing pushing an empty entry defeats the point
		if (entry.empty())
			return;

		// Just update the thumbnail if the top entry has the same path
		if (!empty() && mru_.back().filePath() == entry.filePath()) {
			mru_.back().setThumbnail(entry.thumbnail());
			return;
		}

		pushPrivate(std::move(entry));
	}

	ReferenceType front()
	{
		return mru_.back();
	}

	ConstReferenceType front() const
	{
		return mru_.back();
	}

	ReferenceType back()
	{
		return mru_.front();
	}

	ConstReferenceType back() const
	{
		return mru_.front();
	}

	IteratorType begin()
	{
		return mru_.rbegin();
	}

	ConstIteratorType begin() const
	{
		return mru_.crbegin();
	}

	IteratorType end()
	{
		return mru_.rend();
	}

	ConstIteratorType end() const
	{
		return mru_.crend();
	}

	void clear()
	{
		mru_.clear();
	}

	bool empty() const
	{
		return mru_.empty();
	}

	qsizetype count() const
	{
		return mru_.count();
	}

	qsizetype max() const
	{
		return size_;
	}

private:
	void pushPrivate(MruEntry&& incoming);

	ListType mru_;
	qsizetype size_;
};

} // end namespace MosConfig
