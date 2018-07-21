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

#include "channelmodel.h"

namespace Twitch
{
ChannelModel::ChannelModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int ChannelModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return m_data.size();
}

QVariant ChannelModel::data(const QModelIndex& index, int role) const
{
    switch (role)
    {
    case Id:
        return QString(m_data[index.row()].id);
    case DisplayName:
        return m_data[index.row()].displayName;
    case Status:
        return m_data[index.row()].status;
    case Live:
        return m_data[index.row()].live;
    case Url:
        return m_data[index.row()].url;
    case Preview:
        return m_data[index.row()].preview;
    case Logo:
        return m_data[index.row()].logo;
    case Game:
        return m_data[index.row()].game;
    case Viewers:
        return m_data[index.row()].viewers;
    }

    return {};
}

QHash<int, QByteArray> ChannelModel::roleNames() const
{
    const static QHash<int, QByteArray> map = {
        { Id, "channelId" },
        { DisplayName, "displayName" },
        { Status, "status" },
        { Live, "live" },
        { Url, "url" },
        { Preview, "preview" },
        { Logo, "logo" },
        { Game, "game" },
        { Viewers, "viewers" },
    };

    return map;
}

void ChannelModel::updateChannels(ChannelList channels)
{
    for (const auto& channel : channels)
        updateChannel(channel);

    sortChannels();
}

void ChannelModel::updateChannel(Channel channel)
{
    // find channel in our list
    int row = 0;
    for (auto& d : m_data)
    {
        if (d.id == channel.id)
        {
            QVector<int> changedRoles;

            updateRole(DisplayName, changedRoles, d.displayName, channel.displayName);
            updateRole(Status, changedRoles, d.status, channel.status);
            updateRole(Url, changedRoles, d.url, channel.url);
            updateRole(Logo, changedRoles, d.logo, channel.logo);

            emit dataChanged(index(row), index(row), changedRoles);
        }

        row++;
    }
}

void ChannelModel::updateStreams(StreamList streams)
{
    for (const auto& stream : streams)
        updateStream(stream);

    int row = 0;
    for (auto& d : m_data)
    {
        bool found = false;
        for (auto& s : streams)
        {
            if (d.id == s.channel.id)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            QVector<int> changedRoles;
            updateRole(Live, changedRoles, d.live, false);
            emit dataChanged(index(row), index(row), changedRoles);
            emit liveStatusChanged(d.id);
        }

        row++;
    }

    updateLiveChannelCount();
    sortChannels();
}

void ChannelModel::updateStream(Stream stream)
{
    // find channel in our list
    int row = 0;
    for (auto& d : m_data)
    {
        if (d.id == stream.channel.id)
        {
            QVector<int> changedRoles;

            updateRole(Preview, changedRoles, d.preview, stream.preview);
            updateRole(Viewers, changedRoles, d.viewers, stream.viewers);
            updateRole(Game, changedRoles, d.game, stream.game);
            updateRole(Live, changedRoles, d.live, true);

            if (changedRoles.contains(Live))
                emit liveStatusChanged(d.id);

            emit dataChanged(index(row), index(row), changedRoles);

            updateChannel(stream.channel);

            return;
        }

        row++;
    }
}

void ChannelModel::addChannel(Channel channel)
{
    for (const auto& c : qAsConst(m_data))
    {
        if (c.id == channel.id)
            return;
    }

    ChannelData channelData;
    channelData.id          = channel.id;
    channelData.displayName = channel.displayName;
    channelData.logo        = channel.logo;

    beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
    m_data << channelData;
    endInsertRows();
}

void ChannelModel::addChannelId(ID channelId, QString displayName)
{
    Channel data;
    data.id          = channelId;
    data.displayName = displayName;

    addChannel(data);
}

void ChannelModel::removeChannel(QString channelId)
{
    for (int i = 0; i < m_data.size(); ++i)
    {
        if (m_data[i].id == channelId)
        {
            beginRemoveRows(QModelIndex(), i, i);
            m_data.removeAt(i);
            endRemoveRows();

            return;
        }
    }
}

void ChannelModel::save(QSettings* settings) const
{
    if (!settings)
        return;

    settings->beginGroup("channels");
    settings->beginWriteArray("channel");
    for (int i = 0; i < m_data.size(); ++i)
    {
        settings->setArrayIndex(i);
        settings->setValue("id", m_data[i].id);
        settings->setValue("displayName", m_data[i].displayName);
    }
    settings->endArray();
    settings->endGroup();
}

void ChannelModel::load(QSettings* settings)
{
    if (!settings)
        return;

    clear();

    settings->beginGroup("channels");
    int size = settings->beginReadArray("channel");
    for (int i = 0; i < size; ++i)
    {
        settings->setArrayIndex(i);
        addChannelId(settings->value("id").toString(), settings->value("displayName").toString());
    }
    settings->endArray();
    settings->endGroup();
}

ChannelData ChannelModel::channelData(ID channelId) const
{
    for (const auto& c : qAsConst(m_data))
    {
        if (c.id == channelId)
            return c;
    }

    return {};
}

IDList ChannelModel::ids()
{
    IDList ids;
    for (const auto& c : qAsConst(m_data))
        ids << c.id;

    return ids;
}

void ChannelModel::clear()
{
    beginResetModel();
    m_data.clear();
    endResetModel();
}

int ChannelModel::liveChannelCount() const
{
    return m_liveChannelCount;
}

void ChannelModel::setLiveChannelCount(int liveChannelCount)
{
    if (m_liveChannelCount != liveChannelCount)
    {
        m_liveChannelCount = liveChannelCount;
        emit liveChannelCountChanged();
    }
}

void ChannelModel::sortChannels()
{
    auto lessThan = [](const ChannelData& left, const ChannelData& right) {
        const auto leftName  = left.displayName;
        const auto rightName = right.displayName;

        const auto leftLive  = left.live;
        const auto rightLive = right.live;

        if (leftLive && !rightLive)
        {
            return true;
        }
        else if (!leftLive && rightLive)
        {
            return false;
        }
        else if ((leftLive && rightLive) || (!leftLive && !rightLive))
        {
            return leftName < rightName;
        }

        return false;
    };

    beginResetModel();
    std::sort(m_data.begin(), m_data.end(), lessThan);
    endResetModel();
}

void ChannelModel::updateLiveChannelCount()
{
    int liveCount = 0;
    for (const auto& c : qAsConst(m_data))
    {
        if (c.live)
            liveCount++;
    }

    setLiveChannelCount(liveCount);
}
}
