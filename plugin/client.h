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

#include "channelmodel.h"
#include "data.h"

#include <QObject>
#include <QtNetwork>

namespace Twitch
{
class Client : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString clientID READ clientID WRITE setClientID NOTIFY clientIDChanged)
    Q_PROPERTY(ChannelModel* model READ model NOTIFY modelChanged)
    Q_PROPERTY(ChannelModel* searchModel READ searchModel NOTIFY searchModelChanged)
    Q_PROPERTY(QDateTime lastUpdate READ lastUpdate NOTIFY lastUpdateChanged)
    Q_PROPERTY(bool showNotifications READ showNotifications WRITE setShowNotifications NOTIFY showNotificationsChanged)
    Q_PROPERTY(bool searchInProgress READ searchInProgress WRITE setSearchInProgress NOTIFY searchInProgressChanged)

public:
    explicit Client(const QString clientID = "", QObject* parent = nullptr);

    QString clientID() const;
    void setClientID(const QString& clientID);

    Q_INVOKABLE void searchChannel(const QString& query);
    void queryStreams(const IDList& channelIDs);
    void queryChannels(const IDList& channelIDs);

    ChannelModel* model() const;
    ChannelModel* searchModel() const;

    void updateStreams();
    void updateChannels();

    QDateTime lastUpdate() const;

    bool showNotifications() const;
    void setShowNotifications(bool showNotifications);

    bool searchInProgress() const;
    void setSearchInProgress(bool searchInProgress);

    Q_INVOKABLE void exec(const QString& cmd);

signals:
    void clientIDChanged();
    void searchChannelFinishedJS(QString pattern, QList<QVariant> results);
    void searchChannelFinished(QString pattern, ChannelList results);
    void searchUserIDsFinished(QStringList ids, UserList results);
    void queryStreamsFinished(IDList channelIDs, StreamList results);

    void modelChanged();
    void searchModelChanged();
    void lastUpdateChanged();
    void showNotificationsChanged();
    void searchInProgressChanged();
    void watchChannelRequested(QString url);

protected:
    QNetworkRequest prepareRequest(const QString& endpoint, const QVariantMap& queries);
    QUrl queryUrl(const QString& endpoint, const QVariantMap& queries) const;
    QJsonObject parseJson(QNetworkReply* reply) const;
    void hookupErrorHandlers(QNetworkReply* reply) const;
    QString joinIDs(const IDList& ids) const;
    void setLastUpdate(const QDateTime& lastUpdate);

protected slots:
    void update();
    void liveStatusChanged(ID channelId);

private:
    QString m_clientID;
    QNetworkAccessManager m_network;
    ChannelModel* m_model       = nullptr;
    ChannelModel* m_searchModel = nullptr;
    QTimer* m_updateTimer       = nullptr;
    QTimer* m_forceUpdateTimer  = nullptr;
    bool m_showNotifications    = true;
    QDateTime m_lastUpdate;
    bool m_searchInProgress = false;
};
}
