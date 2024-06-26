/*
    This file is part of the KDE project
    SPDX-FileCopyrightText: 2013 Boudewijn Rempt <boud@valdyas.org>

    SPDX-License-Identifier: LGPL-2.0-or-later

    This code is based on Thacher Ulrich PSD loading code released
    on public domain. See: http://tulrich.com/geekstuff/
*/

#include "ora.h"

#include <QImage>
#include <QScopedPointer>

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

static constexpr char s_magic[] = "image/openraster";
static constexpr int s_magic_size = sizeof(s_magic) - 1; // -1 to remove the last \0

OraHandler::OraHandler()
{
}

bool OraHandler::canRead() const
{
    if (canRead(device())) {
        setFormat("ora");
        return true;
    }
    return false;
}

bool OraHandler::read(QImage *image)
{
	QuaZip zip(device());
	if (!zip.open(QuaZip::mdUnzip)) {
        return false;
    }

	zip.setCurrentFile("mergedimage.png");

	if(!zip.hasCurrentFile()) {
		return false;
	}

	QuaZipFile file(&zip);
	if (!file.open(QIODevice::ReadOnly)) {
		return false;
	}

	image->loadFromData(file.readAll(), "PNG");

	return true;
}

bool OraHandler::canRead(QIODevice *device)
{
    if (!device) {
        qWarning("OraHandler::canRead() called with no device");
        return false;
    }
    if (device->isSequential()) {
        return false;
    }

    char buff[54];
    if (device->peek(buff, sizeof(buff)) == sizeof(buff)) {
        return memcmp(buff + 0x26, s_magic, s_magic_size) == 0;
    }

    return false;
}

QImageIOPlugin::Capabilities OraPlugin::capabilities(QIODevice *device, const QByteArray &format) const
{
    if (format == "ora" || format == "ORA") {
        return Capabilities(CanRead);
    }
    if (!format.isEmpty()) {
        return {};
    }
    if (!device->isOpen()) {
        return {};
    }

    Capabilities cap;
    if (device->isReadable() && OraHandler::canRead(device)) {
        cap |= CanRead;
    }
    return cap;
}

QImageIOHandler *OraPlugin::create(QIODevice *device, const QByteArray &format) const
{
    QImageIOHandler *handler = new OraHandler;
    handler->setDevice(device);
    handler->setFormat(format);
    return handler;
}

#include "moc_ora.cpp"
