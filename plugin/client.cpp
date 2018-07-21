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

#include "client.h"
#include "definitions.h"

#include <KNotification>

namespace Twitch
{

Client::Client(const QString clientID, QObject* parent)
    : QObject(parent)
    , m_clientID(clientID)
    , m_model(new ChannelModel(this))
    , m_searchModel(new ChannelModel(this))
    , m_updateTimer(new QTimer(this))
    , m_forceUpdateTimer(new QTimer(this))
{
    setClientID(TWITCH_CLIENT_ID);

    emit modelChanged();
    emit searchModelChanged();

    connect(m_model, &ChannelModel::rowsInserted, this, [this]() {
        m_forceUpdateTimer->start();
    });

    connect(m_model, &ChannelModel::liveStatusChanged, this, &Client::liveStatusChanged);

    m_updateTimer->setInterval(1000 * 60 * 10); // 10min
    m_updateTimer->setSingleShot(false);
    m_updateTimer->start();
    connect(m_updateTimer, &QTimer::timeout, this, &Client::update);

    m_forceUpdateTimer->setInterval(0);
    m_forceUpdateTimer->setSingleShot(true);
    connect(m_forceUpdateTimer, &QTimer::timeout, this, [this]() {
        updateChannels();
        updateStreams();
    });
}

QString Client::clientID() const
{
    return m_clientID;
}

void Client::setClientID(const QString& clientID)
{
    if (m_clientID != clientID)
    {
        m_clientID = clientID;
        emit clientIDChanged();
    }
}

void Client::searchChannel(const QString& query)
{
    searchModel()->clear();
    setSearchInProgress(true);

    auto reply = m_network.get(prepareRequest("search/channels", { { "query", query } }));

    connect(reply, &QNetworkReply::finished, this, [this, query, reply]() {
        auto obj = parseJson(reply);

        ChannelList channels;

        for (auto o : obj.value("channels").toArray())
            channels << Channel(o.toObject());

        emit searchChannelFinished(query, channels);

        m_searchModel->clear();
        for (auto c : channels)
        {
            m_searchModel->addChannel(c);
        }

        setSearchInProgress(false);

        reply->deleteLater();
    });

    hookupErrorHandlers(reply);
}

void Client::queryStreams(const IDList& channelIDs)
{
    if (channelIDs.empty())
        return;

    auto reply = m_network.get(prepareRequest("streams/", { { "channel", joinIDs(channelIDs) } }));

    // qDebug() << "Query:" << joinIDs(channelIDs) << reply->url();

    connect(reply, &QNetworkReply::finished, this, [this, channelIDs, reply]() {
        auto obj = parseJson(reply);

        StreamList streams;

        for (auto o : obj.value("streams").toArray())
            streams << Stream(o.toObject());

        emit queryStreamsFinished(channelIDs, streams);

        m_model->updateStreams(streams);
        setLastUpdate(QDateTime::currentDateTime());

        reply->deleteLater();
    });

    hookupErrorHandlers(reply);
}

void Client::queryChannels(const IDList& channelIDs)
{
    for (auto id : channelIDs)
    {
        auto reply = m_network.get(prepareRequest("channels/" + id, {}));

        connect(reply, &QNetworkReply::finished, this, [this, channelIDs, reply]() {
            auto obj = parseJson(reply);

            Channel channel = obj;
            m_model->updateChannel(channel);

            reply->deleteLater();
        });

        hookupErrorHandlers(reply);
    }
}

QNetworkRequest Client::prepareRequest(const QString& endpoint, const QVariantMap& queries)
{
    QNetworkRequest request(queryUrl(endpoint, queries));
    request.setRawHeader("Accept", "application/vnd.twitchtv.v5+json");
    request.setRawHeader("Client-ID", m_clientID.toLocal8Bit());
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());

    return request;
}

QUrl Client::queryUrl(const QString& endpoint, const QVariantMap& queries) const
{
    QUrlQuery query;

    for (auto key : queries.keys())
    {
        auto var = queries.value(key);

        if (var.type() == QVariant::StringList)
        {
            for (auto s : var.toStringList())
                query.addQueryItem(key, s);
        }
        else if (var.type() == QVariant::String)
        {
            query.addQueryItem(key, var.toString());
        }
    }

    QUrl url = QString(TWITCH_API_URL) + endpoint;
    url.setQuery(query);

    return url;
}

QJsonObject Client::parseJson(QNetworkReply* reply) const
{
    if (!reply)
        return {};

    auto data              = reply->readAll();
    QJsonDocument document = QJsonDocument::fromJson(data);

    return document.object();
}

void Client::hookupErrorHandlers(QNetworkReply* reply) const
{
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), this, [reply](QNetworkReply::NetworkError code) {
        qDebug() << "Network request error:" << code << reply->errorString();
        reply->deleteLater();
    });

    connect(reply, &QNetworkReply::sslErrors, this, [reply](QList<QSslError> errors) {
        Q_UNUSED(errors)
        qDebug() << "Network request SSL error";
        reply->deleteLater();
    });
}

QString Client::joinIDs(const IDList& ids) const
{
    QString out;

    for (auto id : ids)
    {
        out += id + ',';
    }

    out.chop(1);

    return out;
}

void Client::update()
{
    updateStreams();
}

void Client::liveStatusChanged(ID channelId)
{
    if (m_showNotifications)
    {
        auto channelData = m_model->channelData(channelId);

        if (channelData.live)
        {
            const QString message = QString(tr("<b>%1</b> is now streaming")).arg(channelData.displayName);
            const QString title   = QString(tr("Twitch: %1")).arg(channelData.displayName);

            KNotification* notification = new KNotification("TwitchNowStreaming", KNotification::CloseOnTimeout, this);
            notification->setComponentName("twitchPlasmoid");
            notification->setTitle(title); // use individual title to prevent notifications from getting combined
            notification->setText(message);
            notification->setActions(QStringList() << tr("Watch"));
            connect(notification, &KNotification::action1Activated, this, [this, channelData]() {
                emit watchChannelRequested(channelData.url);
            });
            notification->sendEvent();
        }
    }
}

bool Client::searchInProgress() const
{
    return m_searchInProgress;
}

void Client::setSearchInProgress(bool searchInProgress)
{
    if (m_searchInProgress != searchInProgress)
    {
        m_searchInProgress = searchInProgress;
        emit searchInProgressChanged();
    }
}

void Client::exec(const QString& cmd)
{
    QProcess::startDetached(cmd);
}

bool Client::showNotifications() const
{
    return m_showNotifications;
}

void Client::setShowNotifications(bool showNotifications)
{
    if (m_showNotifications != showNotifications)
    {
        m_showNotifications = showNotifications;
        emit showNotificationsChanged();
    }
}

QDateTime Client::lastUpdate() const
{
    return m_lastUpdate;
}

void Client::setLastUpdate(const QDateTime& lastUpdate)
{
    if (lastUpdate != m_lastUpdate)
    {
        m_lastUpdate = lastUpdate;
        emit lastUpdateChanged();
    }
}

ChannelModel* Client::searchModel() const
{
    return m_searchModel;
}

void Client::updateStreams()
{
    auto ids = m_model->ids();
    queryStreams(ids);
}

void Client::updateChannels()
{
    auto ids = m_model->ids();
    queryChannels(ids);
}

ChannelModel* Client::model() const
{
    return m_model;
}
}
