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

#include <QJsonObject>
#include <QMetaType>

namespace Twitch
{
using ID = QString;

using IDList = QList<ID>;

struct Channel
{
    Channel() {}
    Channel(const QJsonObject& data);

    int views     = 0;
    int followers = 0;
    ID id;
    bool live = false;
    QString displayName;
    QString logo;
    QString game;
    QString status;
    QString url;
};

using ChannelList = QList<Channel>;

struct Stream
{
    Stream() {}
    Stream(const QJsonObject& data);

    ID id;
    int viewers = 0;
    Channel channel;
    QString preview;
    QString game;
};

using StreamList = QList<Stream>;

struct User
{
    User() {}
    User(const QJsonObject& data);

    ID id;
    QString login;
    QString offlineImageUrl;
    QString profileImageUrl;
    QString description;
    QString displayName;
    int viewCount = 0;
};

using UserList = QList<User>;
}

Q_DECLARE_METATYPE(Twitch::ID);
Q_DECLARE_METATYPE(Twitch::IDList);
Q_DECLARE_METATYPE(Twitch::Channel);
Q_DECLARE_METATYPE(Twitch::ChannelList);
Q_DECLARE_METATYPE(Twitch::Stream);
Q_DECLARE_METATYPE(Twitch::StreamList);
