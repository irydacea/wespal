/*
 * Wespal (codename Morning Star) - Wesnoth assets recoloring tool
 *
 * Copyright (C) 2010 - 2024 by Iris Morelle <iris@irydacea.me>
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

#include "util.hpp"

#include "version.hpp"

#include <QAbstractSlider>
#include <QDesktopServices>
#include <QFileInfo>
#include <QImageReader>
#include <QMessageBox>
#include <QStandardPaths>
#include <QStringBuilder>
#include <QToolTip>

namespace {

const QString ABOUT_AUTHOR_URL = QStringLiteral("https://irydacea.me/");

const QString BUG_REPORTS_URL = QStringLiteral("https://wespal.irydacea.me/issues");

const QString CHANGELOG_URL_TEMPLATE = QStringLiteral("https://wespal.irydacea.me/release/%1");

const QString LICENSE_URL = QStringLiteral("https://www.gnu.org/licenses/old-licenses/gpl-2.0.html#SEC1");

/** Namespace wrapper for QObject::tr(). */
inline QString tr(const char* text)
{
	return QObject::tr(text);
}

bool doJobNotification(QWidget* parent, const QString& message, const QString& details, QMessageBox::Icon icon)
{
	QMessageBox msg{parent};

	msg.setWindowTitle(tr("Wespal"));
	msg.setText(message);
	msg.setDetailedText(details);
	msg.setIcon(icon);
	msg.setWindowModality(Qt::WindowModal);

	if (icon == QMessageBox::Question) {
		msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	}

	const QMessageBox::StandardButton result =
		QMessageBox::StandardButton(msg.exec());

	return icon == QMessageBox::Question
			? result == QMessageBox::Yes
			: true;
}

bool doJobNotification(QWidget* parent, const QString& message, const QStringList& detailItems, QMessageBox::Icon icon)
{
	QString joinedList = detailItems.join("\n");
	return doJobNotification(parent, message, joinedList, icon);
}

} // end anonymous namespace

namespace MosUi {

void message(QWidget* parent, const QString& message, const QString& details)
{
	doJobNotification(parent, message, details, QMessageBox::Information);
}

void message(QWidget* parent, const QString& message, const QStringList& detailItems)
{
	doJobNotification(parent, message, detailItems, QMessageBox::Information);
}

bool prompt(QWidget* parent, const QString& message, const QString& details)
{
	return doJobNotification(parent, message, details, QMessageBox::Question);
}

bool prompt(QWidget* parent, const QString& message, const QStringList& detailItems)
{
	return doJobNotification(parent, message, detailItems, QMessageBox::Question);
}

void error(QWidget* parent, const QString& message, const QString& details)
{
	doJobNotification(parent, message, details, QMessageBox::Critical);
}

void error(QWidget* parent, const QString& message, const QStringList& detailItems)
{
	doJobNotification(parent, message, detailItems, QMessageBox::Critical);
}

void about(QWidget* parent)
{
	QString text =
		tr("<h3>Wespal %1</h3>").arg(MOS_VERSION) %
		"<p>" %
		tr(
			"Copyright &copy; 2008 &#8211; 2024 by "
			"<a href='%1'>Iris Morelle</a>").arg(ABOUT_AUTHOR_URL) %
		"</p><p>" %
		tr("Built with Qt %1").arg(QT_VERSION_STR) %
		"</p><p>" %
		tr(
			"This program is free software; you can redistribute it and/or modify "
			"it under the terms of the GNU General Public License as published by "
			"the Free Software Foundation; either version 2 of the License, or "
			"(at your option) any later version.") %
		"</p><p>" %
		tr(
			"This program is distributed in the hope that it will be useful, but "
			"<b>WITHOUT ANY WARRANTY</b>; without even the implied warranty of "
			"<b>MERCHANTABILITY</b> or <b>FITNESS FOR A PARTICULAR PURPOSE</b>. "
			"See the <a href='%1'>"
			"GNU General Public License</a> for more details.").arg(LICENSE_URL) %
		"</p>";
	QMessageBox::about(parent, tr("Wespal"), text);
}

void openReleaseNotes()
{
	static const bool isDevVersion = MOS_VERSION.endsWith("-dev");
	static const QString changeLogUrl =
			CHANGELOG_URL_TEMPLATE.arg(isDevVersion ? "dev" : MOS_VERSION);
	QDesktopServices::openUrl({changeLogUrl});
}

void openIssueTracker()
{
	QDesktopServices::openUrl({BUG_REPORTS_URL});
}

void displaySliderTextToolTip(QAbstractSlider* slider, int newValue, const QString& text)
{
	if (!slider)
		return;

	auto percentageFactor = double(newValue - slider->minimum()) / double(slider->maximum() - slider->minimum());
	const QString& toolTipText = !text.isEmpty() ? text : QString{"%1%"}.arg(qRound(100.0 * percentageFactor));

	auto toolTipPos = slider->mapToGlobal(QPoint{0, 0});

	if (slider->orientation() == Qt::Horizontal)
		toolTipPos += QPoint{qRound(slider->width() * percentageFactor), 0};
	else
		toolTipPos += QPoint{slider->width(), qRound(slider->height() * percentageFactor)};

	//qDebug() << toolTipPos << percentageFactor;

	QToolTip::showText(toolTipPos, toolTipText, slider);
}

} // end namespace MosUi

namespace MosPlatform {

QString supportedImageFileFormats()
{
	QString res;

	QList<QPair<QString, QString>> supportedFormats = {
		{ "*.png",  tr("PNG image") },
		{ "*.webp", tr("WebP image") },
		{ "*.bmp",  tr("Windows Bitmap") },
	};

	static const QMap<QString, QString> pluginFormats = {
		{ "*.xcf",  tr("GIMP image") },
		{ "*.psd",  tr("Photoshop image") },
		{ "*.ora",  tr("OpenRaster image") },
		{ "*.kra",  tr("Krita image") },
	};

	for (const auto& format : QImageReader::supportedImageFormats())
	{
		QString str = QStringLiteral("*.") % format;

		auto it = pluginFormats.find(str.toLower());

		if (it != pluginFormats.end()) {
			supportedFormats.emplaceBack(it.key(), it.value());
		}
	}

	QStringList entries;
	entries.reserve(2 + supportedFormats.count());
	entries << tr("All Supported Files") % " (" ;
	QString& catchAll = entries.front();

	for (const auto& [pattern, label] : supportedFormats)
	{
		if (entries.count() > 1)
			catchAll += ' ';
		catchAll += pattern;
		entries << label % QStringLiteral(" (") % pattern % ')';
	}

	catchAll += ')';
	entries << tr("All Files") % QStringLiteral(" (*)");
	res = entries.join(QStringLiteral(";;"));

	return res;
}

QString desktopPicturesFolderPath()
{
	const auto& pictureLocs = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);

	// Use pwd as a fallback if somehow the desktop platform has
	// no idea where pictures are usually stored
	return !pictureLocs.empty()
			? pictureLocs.first()
			: QFileInfo{"."}.absolutePath();
}

} // end namespace MosPlatform

QString cleanFileName(const QString& fileName)
{
	QFileInfo qfi{fileName};

	return qfi.baseName() + '.' + qfi.completeSuffix();
}
