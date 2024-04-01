/*
 * Wespal (codename Morning Star) - Wesnoth assets recoloring tool
 *
 * Copyright (C) 2024 by Iris Morelle <iris@irydacea.me>
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

#include "recentfiles.hpp"

#include <algorithm>

namespace MosConfig {

namespace {

constexpr QSize MRU_THUMBNAIL_SIZE = { 72, 72 };

constexpr QSize MRU_MINI_THUMBNAIL_SIZE = { 16, 16 };

} // end unnamed namespace

const QSize& MruEntry::thumbnailSize()
{
	return MRU_THUMBNAIL_SIZE;
}

const QSize& MruEntry::miniThumbnailSize()
{
	return MRU_MINI_THUMBNAIL_SIZE;
}

MruEntry::MruEntry(const QString& filePath, const QString& thumbnailData)
	: filePath_(filePath)
	, thumbnail_()
{
	auto rawThumbnail = QByteArray::fromBase64(thumbnailData.toLatin1());
	thumbnail_ = QImage::fromData(rawThumbnail);

	if (thumbnail_.size() != MRU_THUMBNAIL_SIZE) {
		thumbnail_ = thumbnail_.scaled(MRU_THUMBNAIL_SIZE,
									   Qt::KeepAspectRatio,
									   Qt::SmoothTransformation);
	}

	miniThumbnail_ = thumbnail_.scaled(MRU_MINI_THUMBNAIL_SIZE,
									   Qt::KeepAspectRatio,
									   Qt::FastTransformation);
}

MruEntry::MruEntry(const QString& filePath, const QImage& image)
	: filePath_(filePath)
	, thumbnail_()
{
	if (!image.isNull()) {
		thumbnail_ = image.scaled(MRU_THUMBNAIL_SIZE,
								  Qt::KeepAspectRatio,
								  Qt::SmoothTransformation);
		miniThumbnail_ = image.scaled(MRU_MINI_THUMBNAIL_SIZE,
									  Qt::KeepAspectRatio,
									  Qt::FastTransformation);
	}
}

void MruList::pushPrivate(MruEntry&& incoming)
{
	if (!mru_.empty()) {
		auto first = mru_.begin();
		auto oldLast = mru_.end();

		// Remove duplicates
		auto newLast = std::remove_if(first, oldLast, [&](const MruEntry& e) {
			return e.filePath() == incoming.filePath();
		});

		mru_.erase(ListType::ConstIterator{newLast}, mru_.cend());
	}

	mru_.emplaceBack(incoming);

	// Shift everything back
	if (mru_.count() > size_) {
		auto newEnd = std::move(mru_.begin() + (mru_.count() - size_),
								mru_.end(),
								mru_.begin());
		mru_.erase(ListType::ConstIterator{newEnd}, mru_.cend());
	}

	Q_ASSERT(mru_.count() <= size_);
}

} // end namespace MosConfig
