/*****************************************************************************
 *                                                                           *
 *  Copyright (c) 2016-2018 Boris Pek <tehnick-8@yandex.ru>                  *
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

#pragma once

#include <QString>
#include <QByteArray>
#include <QCryptographicHash>

#include "abstractsettings.h"
#include "request.h"

class UserSettings : public AbstractSettings
{
    friend class CommandLineDialogs;
    friend class ProfileSettingsSecurityPage;

public:
    explicit UserSettings();
    explicit UserSettings(const QString &fileName);
    UserSettings(const UserSettings &in) = delete;
    UserSettings(UserSettings &&in) = delete;
    UserSettings& operator=(const UserSettings &in) = delete;
    virtual ~UserSettings() = default;

    bool isValidAutorizationRequest(const Request &request);
    QByteArray getAvatarUrl() const;
    QByteArray gravatarIconUrl() const;
    QString getUserRole(const QString &projectName) const;

    static QString generatePasswordHash(const QString &password);

protected:
    bool checkPasswordHash(const QString &password) const;

private:
    static bool slowEquals(const QByteArray &a, const QByteArray &b);
    static QByteArray pbkdf2(const QCryptographicHash::Algorithm method,
                             const QByteArray &password,
                             const QByteArray &salt,
                             const int rounds,
                             const int keyLength);
    static QByteArray hash(const QByteArray &password,
                           const QByteArray &salt,
                           const int rounds);
    static QByteArray randomSalt();
    static QByteArray calcEmailHash(const QString &email);
    static QByteArray calcUserIdHash(const QString &id);
};

