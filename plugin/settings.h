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

#pragma once

#include <QSettings>

class Settings : public QSettings
{
    Q_OBJECT
    Q_PROPERTY(bool watchInBrowser READ watchInBrowser WRITE setWatchInBrowser NOTIFY watchInBrowserChanged)
    Q_PROPERTY(QString cmdOnWatch READ cmdOnWatch WRITE setCmdOnWatch NOTIFY cmdOnWatchChanged)
public:
    explicit Settings(QObject* parent = nullptr);

    bool watchInBrowser();
    void setWatchInBrowser(bool browser);
    QString cmdOnWatch();
    void setCmdOnWatch(const QString& cmd);
    Q_INVOKABLE QString cmdOnWatchArg(const QString& arg);

signals:
    void watchInBrowserChanged();
    void cmdOnWatchChanged();

private:
    bool m_watchInBrowser = true;
    QString m_cmdOnWatch;
};
