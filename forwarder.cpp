#include "forwarder.h"

#include <QTextStream>
#include <QNetworkAccessManager>
#include <QTcpSocket>
#include <QUrl>

Forwarder::Forwarder(QTcpSocket *client, QObject *parent)
    : QObject(parent)
    , m_client(client)
    , m_proxy(nullptr)
    , m_tunnel(nullptr)
{
    connect(client, &QTcpSocket::readyRead, this, &Forwarder::handleRequest);
    connect(client, &QTcpSocket::disconnected, this, &Forwarder::deleteLater);
}

void Forwarder::setProxy(QTcpSocket *proxy)
{
    if (m_proxy) {
        this->disconnect(m_proxy);
        m_proxy->disconnect(this);
    }

    m_proxy = proxy;
    if (proxy) {
        connect(m_client, &QTcpSocket::disconnected, proxy, &QTcpSocket::disconnectFromHost);
        connect(proxy, &QTcpSocket::readyRead, this, &Forwarder::forwardResponse);
        connect(proxy, &QTcpSocket::disconnected, m_client, &QTcpSocket::disconnectFromHost);
    }
}

void Forwarder::handleRequest()
{
    if (m_proxy) {
        QByteArray data = m_client->readAll();
        filterRequest(data, true);
        m_proxy->write(data);
        return;
    }

    while (!m_client->atEnd()) {
        QByteArray data = m_client->readLine();

        QTextStream stream(&data, QIODevice::ReadOnly);
        QByteArray method, rawUrl, protocol;
        stream >> method >> rawUrl >> protocol;

        //Read the whole request
        data.clear();
        forever {
            QByteArray line = m_client->readLine();
            data.append(line);
            if (line == "\r\n" || line.isEmpty())
                break;
        }

        if (method == "GET" || method == "POST") {
            //Resolve host
            QUrl url = QUrl::fromEncoded(rawUrl);
            QString host = url.host();
            ushort port = url.port(80);

            QTcpSocket *socket = new QTcpSocket(this);
            connect(socket, &QTcpSocket::readyRead, this, &Forwarder::forwardResponse);
            connect(socket, &QTcpSocket::connected, [=](){
                socket->write(method);
                socket->write(" ");

                QString relativeUrl = url.path();
                if (url.hasQuery()) {
                    relativeUrl.append('?');
                    relativeUrl.append(url.query());
                }
                socket->write(relativeUrl.toLatin1());
                socket->write(" ");
                socket->write(protocol);
                socket->write("\r\n");
                socket->write(data);
            });
            connect(socket, (void (QAbstractSocket::*)(QAbstractSocket::SocketError)) &QTcpSocket::error, [=](QAbstractSocket::SocketError){
                m_client->disconnectFromHost();
            });
            socket->connectToHost(host, port);
        } else {
            disconnect(m_client, &QTcpSocket::readyRead, this, &Forwarder::handleRequest);

            m_tunnel = new QTcpSocket(this);

            connect(m_tunnel, &QTcpSocket::readyRead, this, &Forwarder::forwardResponse);
            connect(m_tunnel, &QTcpSocket::connected, [&](){
                connect(m_client, &QTcpSocket::readyRead, this, &Forwarder::forwardRequest);
                m_client->write("HTTP/1.1 200 Connection Established\r\n\r\n");
            });
            connect(m_tunnel, (void (QAbstractSocket::*)(QAbstractSocket::SocketError)) &QTcpSocket::error, [=](QAbstractSocket::SocketError){
                m_client->disconnectFromHost();
            });

            QList<QByteArray> parts = rawUrl.split(':');
            QString host = QString::fromLatin1(parts.at(0));
            ushort port = 80;
            if (parts.length() > 1)
                port = parts.at(1).toUShort();
            m_tunnel->connectToHost(host, port);
        }
    }
}

void Forwarder::filterRequest(QByteArray &data, bool forward)
{
    const char *keywords[][2] = {
        "google", "gu-guru",
        "twitter", "tsuitta-"
    };

    if (forward) {
        for (const char **keyword : keywords) {
            data.replace(keyword[0], keyword[1]);
        }
    } else {
        for (const char **keyword : keywords) {
            data.replace(keyword[1], keyword[0]);
        }
    }
}

void Forwarder::forwardRequest()
{
    if (m_tunnel == nullptr) {
        qWarning("Tunnel is null");
        return;
    }

    while (!m_client->atEnd()) {
        QByteArray data = m_client->readAll();
        m_tunnel->write(data);
    }
}

void Forwarder::forwardResponse()
{
    QTcpSocket *proxy = qobject_cast<QTcpSocket *>(sender());
    while (!proxy->atEnd()) {
        QByteArray data = proxy->readAll();
        m_client->write(data);
    }
}
