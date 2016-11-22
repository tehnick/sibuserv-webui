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

#include "applicationsettings.h"
#include "resourcemanager.h"
#include "datafilewithlimitedaccess.h"

DataFileWithLimitedAccess::DataFileWithLimitedAccess(const Request &request,
                                                     const QString &projectName,
                                                     const QString &fileName) :
    HtmlPage(request)
{
    if (request.get("ajax").isEmpty()) {
        generateHtmlTemplate(projectName, fileName);
        update();
    }
    else {
        generateAjaxResponse(projectName, fileName);
    }
    show();
}

bool DataFileWithLimitedAccess::isAllowedAccess(const QString &projectName,
                                                const QString &fileName) const
{
    if (isAdmin())
        return true;

    const UserSettings &us = userSettings();
    const QJsonObject &&obj = us.getObject("projects");

    if (!obj.contains(projectName))
        return false;

    const QString &&role = obj[projectName].toString();

    if (role == "tester") {
        static const QStringList logFiles = {
            "build.log",
            "cppcheck.log",
            "cppcheck.html"
        };
        for (const auto &f : logFiles) {
            if (fileName.contains(f)) {
                return false;
            }
        }
        return true;
    }

    return (role == "developer" || role == "owner");
}

void DataFileWithLimitedAccess::generateHtmlTemplate(const QString &projectName, const QString &fileName)
{
    if (isAutorizedUser()) {
        if (isAllowedAccess(projectName, fileName)) {
            ResourceManager res(APP_S().cacheDirectory() + "projects");
            res.addPath(APP_S().buildServerBinDir());
            if (res.contains(fileName)) {
                setData(res.read(fileName));
                autodetectContentType(fileName);
            }
            else if (fileName.endsWith(".tar.gz")) {
                ;
            }
        }
        else {
            forbidAccess();
        }
    }
    else {
        forbidAccess();
        forceAuthorization();
    }
}

void DataFileWithLimitedAccess::generateAjaxResponse(const QString &projectName,
                                                     const QString &fileName)
{
    ;
}

