/*****************************************************************************
 *                                                                           *
 *  Copyright (c) 2016 Boris Pek <tehnick-8@yandex.ru>                       *
 *                                                                           *
 *  Permission is hereby granted, free of charge, to any person obtaining    *
 *  a copy of this software and associated documentation files (the          *
 *  "Software"), to deal in the Software without restriction, including      *
 *  without limitation the rights to use, copy, modify, merge, publish,      *
 *  distribute, sublicense, and/or sell copies of the Software, and to       *
 *  permit persons to whom the Software is furnished to do so, subject to    *
 *  the following conditions:                                                *
 *                                                                           *
 *  The above copyright notice and this permission notice shall be included  *
 *  in all copies or substantial portions of the Software.                   *
 *                                                                           *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,          *
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF       *
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   *
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY     *
 *  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,     *
 *  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE        *
 *  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                   *
 *                                                                           *
 *****************************************************************************/

#include <QStringList>
#include <QJsonParseError>
#include <QJsonDocument>

#include "resourcemanager.h"
#include "abstractsettings.h"

struct AbstractSettings::AbstractSettingsPrivate
{
    QString path;
    QString fileName;
    QJsonObject settings;
    QJsonObject defaultSettings;
};

AbstractSettings::AbstractSettings(const QString &path, const QString &fileName) :
    d(new AbstractSettingsPrivate)
{
    setPath(path);
    setFileName(fileName);
    readSettings();
}

AbstractSettings::~AbstractSettings()
{
    delete d;
}

void AbstractSettings::setPath(const QString &path)
{
    if (path.isEmpty())
        return;

    d->path = path;
}

void AbstractSettings::setFileName(const QString &fileName)
{
    if (fileName.isEmpty())
        return;

    d->fileName = fileName;
}

bool AbstractSettings::readSettings()
{
    if (d->path.isEmpty() || d->fileName.isEmpty())
        return false;

    ResourceManager res(":");
    if (res.contains(d->fileName)) {
        readSetting(res.read(d->fileName), d->defaultSettings);
    }
    res.setPath(d->path);
    if (res.contains(d->fileName)) {
        if (readSetting(res.read(d->fileName), d->settings)) {
            return true;
        }
    }

    return false;
}

bool AbstractSettings::writeSettings() const
{
    if (d->path.isEmpty() || d->fileName.isEmpty() || d->settings.isEmpty())
        return false;

    const ResourceManager res;
    const QJsonDocument doc(d->settings);
    return res.write(d->path + "/" + d->fileName, doc.toJson());
}

void AbstractSettings::clear()
{
    for (const auto &key : d->defaultSettings.keys()) {
        d->defaultSettings.remove(key);
    }
    for (const auto &key : d->settings.keys()) {
        d->settings.remove(key);
    }
    d->fileName.clear();
    d->path.clear();
}

void AbstractSettings::setSettings(QJsonObject &object)
{
    d->settings = object;
}

bool AbstractSettings::readSetting(const QByteArray &data,
                                   QJsonObject &settings)
{
    QJsonDocument doc;
    QJsonParseError err;
    doc = QJsonDocument::fromJson(data, &err);

    if (err.error == QJsonParseError::NoError && !doc.isNull()) {
        settings = doc.object();
        return true;
    }

    return false;
}

QStringList AbstractSettings::keys() const
{
    QStringList out = d->settings.keys();

    for (const auto &key : d->defaultSettings.keys()) {
        if (!out.contains(key)) {
            out.append(key);
        }
    }

    return out;
}

bool AbstractSettings::contains(const QString &key) const
{
    if (key.isEmpty())
        return false;

    for (const auto &s: {d->settings, d->defaultSettings}) {
        if (s.contains(key)) {
            return true;
        }
    }

    return false;
}

QString AbstractSettings::get(const QString &key) const
{
    if (key.isEmpty())
        return "";

    for (const auto &s: {d->settings, d->defaultSettings}) {
        if (s.contains(key)) {
            return s[key].toString();
        }
    }

    return "";
}

bool AbstractSettings::getBool(const QString &key) const
{
    if (key.isEmpty())
        return false;

    for (const auto &s: {d->settings, d->defaultSettings}) {
        if (s.contains(key)) {
            return s[key].toBool();
        }
    }

    return false;
}

