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

#include "client.h"

#include <QtTest/QtTest>

class ApiTest : public QObject
{
    Q_OBJECT
public:
    ApiTest();

private slots:
    void searchStreams();

private:
    Twitch::Client m_client;

    Twitch::ChannelList m_channels;
};
