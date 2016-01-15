#include "forwarder.h"

#include <QTextStream>
#include <QNetworkAccessManager>
#include <QTcpSocket>
#include <QUrl>

QMap<QByteArray, Forwarder::CommandHandler> Forwarder::m_handlers;
struct ForwarderInit{
    ForwarderInit(){
        Forwarder::m_handlers["GET"] = &Forwarder::directRequest;
        Forwarder::m_handlers["POST"] = &Forwarder::directRequest;
        Forwarder::m_handlers["CONNECT"] = &Forwarder::setupTunnel;
        Forwarder::m_handlers["CHYOUSA"] = &Forwarder::setFilter;
    }
};
ForwarderInit init;

Forwarder::Forwarder(QTcpSocket *client, QObject *parent)
    : QObject(parent)
    , m_client(client)
    , m_proxy(nullptr)
    , m_tunnel(nullptr)
    , m_enableFilter(false)
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
    while (!m_client->atEnd()) {
        QByteArray line = readLine();

        QTextStream stream(&line, QIODevice::ReadOnly);
        QByteArray method, rawUrl, protocol;
        stream >> method >> rawUrl >> protocol;

        CommandHandler handler = m_handlers.value(method);
        if (handler)
            (this->*handler)(method, rawUrl, protocol);
        else
            qWarning("Unknown method: %s", method.constData());
    }
}

QByteArray Forwarder::readLine()
{
    QByteArray line = m_client->readLine();
    if (m_enableFilter)
        filterRequest(line, false);
    return line;
}

QByteArray Forwarder::readAll()
{
    //Read the whole request
    QByteArray body;
    forever {
        QByteArray line = m_client->readLine();
        body.append(line);
        if (line == "\r\n" || line.isEmpty())
            break;
    }

    if (m_enableFilter)
        filterRequest(body, false);

    return body;
}

bool Forwarder::isBlocked(const QString &domain)
{
#ifdef GESC_OUTSIDER
    Q_UNUSED(domain)
#else
    if (domain.endsWith("google.com"))
        return true;

    if (domain.endsWith("twitter.com"))
        return true;

    if (domain.endsWith("youtube.com"))
        return true;

    if (domain.endsWith("ytimg.com"))
        return true;

    if (domain.endsWith("googlevideo.com"))
        return true;
#endif
    return false;
}

void Forwarder::filterRequest(QByteArray &data, bool forward)
{
    const char *keywords[][2] = {
        "google", "gu-guru",
        "twitter", "tsuitta-",
        "youtube", "fantudou",
        "ytimg", "fantudoudetupian",
        "googlevideo", "gu-gurubidio"
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

void Forwarder::directRequest(const QByteArray &method, const QByteArray &rawUrl, const QByteArray &protocol)
{
    QByteArray body = readAll();

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

        socket->write(body);
    });
    connect(socket, (void (QAbstractSocket::*)(QAbstractSocket::SocketError)) &QTcpSocket::error, [=](QAbstractSocket::SocketError){
        m_client->disconnectFromHost();
    });
    socket->connectToHost(host, port);
}

void Forwarder::setupTunnel(const QByteArray &, const QByteArray &rawUrl, const QByteArray &protocol)
{
    disconnect(m_client, &QTcpSocket::readyRead, this, &Forwarder::handleRequest);

    m_tunnel = new QTcpSocket(this);
    connect(m_tunnel, &QTcpSocket::readyRead, this, &Forwarder::forwardResponse);
    connect(m_tunnel, (void (QAbstractSocket::*)(QAbstractSocket::SocketError)) &QTcpSocket::error, [=](QAbstractSocket::SocketError){
        m_client->disconnectFromHost();
    });

    QList<QByteArray> parts = rawUrl.split(':');
    QString host = QString::fromLatin1(parts.at(0));
    ushort port = 80;
    if (parts.length() > 1)
        port = parts.at(1).toUShort();

    QByteArray body = readAll();

    if (isBlocked(host)) {
        QByteArray url = rawUrl;
        filterRequest(url, true);
        filterRequest(body, true);

        connect(m_tunnel, &QTcpSocket::connected, [=](){
            m_tunnel->write("CHYOUSA true *\r\n");
            m_tunnel->write("CONNECT ");
            m_tunnel->write(url);
            m_tunnel->write(" ");
            m_tunnel->write(protocol);
            m_tunnel->write("\r\n");
            m_tunnel->write(body);

            connect(m_client, &QTcpSocket::readyRead, this, &Forwarder::forwardRequest);
        });
        m_tunnel->connectToHost("p.takashiro.me", 5526);
    } else {
        connect(m_tunnel, &QTcpSocket::connected, [=](){
            connect(m_client, &QTcpSocket::readyRead, this, &Forwarder::forwardRequest);
            m_client->write("HTTP/1.1 200 Connection Established\r\n\r\n");
        });
        m_tunnel->connectToHost(host, port);
    }
}

void Forwarder::setFilter(const QByteArray &, const QByteArray &status, const QByteArray &)
{
    m_enableFilter = (status == "true");
}
