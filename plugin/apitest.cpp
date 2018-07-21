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

#include "apitest.h"
#include "definitions.h"

ApiTest::ApiTest()
{
    m_client.setClientID(TWITCH_CLIENT_ID);

    connect(&m_client, &Twitch::Client::searchChannelFinished, this, [this](QString pattern, Twitch::ChannelList channels) {
        qDebug() << "got channels" << channels.size();

        m_channels = channels;

        Twitch::IDList channelIDs;

        for (auto c : channels)
        {
            qDebug() << c.displayName << c.status;
            channelIDs << c.id;
        }

        m_client.queryStreams(channelIDs);
    });

    connect(&m_client, &Twitch::Client::queryStreamsFinished, this, [this](Twitch::IDList channelIDs, Twitch::StreamList streams) {
        qDebug() << "got streams" << streams.size();
        for (auto c : streams)
        {
            qDebug() << c.id << c.viewers << c.preview;
        }
    });
}

void ApiTest::searchStreams()
{
    m_client.searchChannel("twitch");

    QTest::qWait(1000);

    QVERIFY(!m_channels.empty() && m_channels[0].displayName == "Twitch");
}

QTEST_MAIN(ApiTest)
