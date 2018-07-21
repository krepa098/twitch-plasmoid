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

#include <QAbstractListModel>
#include <QSettings>

#include "data.h"

namespace Twitch
{

struct ChannelData
{
    ID id;
    int viewers = 0;
    bool live   = false;
    QString displayName;
    QString status;
    QString url;
    QString preview;
    QString logo;
    QString game;
};

class ChannelModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int liveChannelCount READ liveChannelCount WRITE setLiveChannelCount NOTIFY liveChannelCountChanged)
public:
    enum Roles
    {
        Id = Qt::UserRole + 1,
        DisplayName,
        Status,
        Live,
        Url,
        Preview,
        Logo,
        Game,
        Viewers,
    };

    explicit ChannelModel(QObject* parent = nullptr);

signals:
    void liveStatusChanged(ID channelId);

public slots:

public:
    virtual int rowCount(const QModelIndex& parent) const;
    virtual QVariant data(const QModelIndex& index, int role) const;
    virtual QHash<int, QByteArray> roleNames() const;
    void updateChannels(ChannelList channels);
    void updateChannel(Channel channel);
    void updateStreams(StreamList streams);
    void updateStream(Stream stream);

    Q_INVOKABLE void addChannel(Channel channel);
    Q_INVOKABLE void addChannelId(QString channelId, QString displayName);
    Q_INVOKABLE void removeChannel(QString channelId);

    Q_INVOKABLE void save(QSettings* settings) const;
    Q_INVOKABLE void load(QSettings* settings);

    ChannelData channelData(ID channelId) const;

    IDList ids();

    Q_INVOKABLE void clear();

    int liveChannelCount() const;
    void setLiveChannelCount(int liveChannelCount);

    void sortChannels();

signals:
    void liveChannelCountChanged();

private:
    template <typename T>
    void updateRole(int role, QVector<int>& changedRoles, T& a, const T& b)
    {
        if (a != b)
        {
            a = b;
            changedRoles << role;
        }
    }

    void updateLiveChannelCount();

private:
    QList<ChannelData> m_data;
    int m_liveChannelCount = 0;
};
}
