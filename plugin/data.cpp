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

#include "data.h"

#include <QVariant>

namespace Twitch
{

// https://dev.twitch.tv/docs/v5
// Note: IDs can (and do) exceed the range of int32

Channel::Channel(const QJsonObject& data)
{
    id          = QString::number(data.value("_id").toVariant().toLongLong());
    views       = data.value("views").toInt();
    followers   = data.value("followers").toInt();
    logo        = data.value("logo").toString();
    displayName = data.value("display_name").toString();
    status      = data.value("status").toString();
    url         = data.value("url").toString();
}

User::User(const QJsonObject& data)
{
    id              = QString::number(data.value("id").toVariant().toLongLong());
    login           = data.value("login").toString();
    offlineImageUrl = data.value("offline_image_url").toString();
    profileImageUrl = data.value("profile_image_url").toString();
    description     = data.value("description").toString();
    displayName     = data.value("display_name").toString();
    viewCount       = data.value("view_count").toInt();
}

Stream::Stream(const QJsonObject& data)
{
    id      = QString::number(data.value("_id").toVariant().toLongLong());
    viewers = data.value("viewers").toInt();
    channel = data.value("channel").toObject();
    preview = data.value("preview").toObject().value("small").toString();
    game    = data.value("game").toString();
}
}
