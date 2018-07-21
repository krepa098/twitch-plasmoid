/*
Copyright (C) 2018  Paul Kremer

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "settings.h"

Settings::Settings(QObject* parent)
    : QSettings(parent)
{
}

bool Settings::watchInBrowser()
{
    auto val = value("watchInBrowser", "true");
    return val.toBool();
}

void Settings::setWatchInBrowser(bool browser)
{
    if (watchInBrowser() != browser)
    {
        setValue("watchInBrowser", browser);

        emit watchInBrowserChanged();
    }
}

QString Settings::cmdOnWatch()
{
    auto val = value("cmdOnWatch", "mpv %1");
    return val.toString();
}

QString Settings::cmdOnWatchArg(const QString& arg)
{
    return cmdOnWatch().arg(arg);
}

void Settings::setCmdOnWatch(const QString& cmd)
{
    if (cmdOnWatch() != cmd)
    {
        setValue("cmdOnWatch", cmd);

        emit cmdOnWatchChanged();
    }
}
