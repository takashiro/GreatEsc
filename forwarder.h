#ifndef FORWARDER_H
#define FORWARDER_H

#include <QObject>
#include <QString>
#include <QMap>

class QTcpSocket;

class Forwarder : public QObject
{
    friend struct ForwarderInit;

public:
    Forwarder(QTcpSocket *client, QObject *parent = nullptr);

    QTcpSocket *proxy() const { return m_proxy; }
    void setProxy(QTcpSocket *proxy);

protected:
    void handleRequest();

    QByteArray readLine();
    QByteArray readAll();

    static bool isBlocked(const QString &domain);
    static void filterRequest(QByteArray &data, bool forward);

    void forwardRequest();
    void forwardResponse();

    QTcpSocket *m_client;
    QTcpSocket *m_proxy;
    QTcpSocket *m_tunnel;
    bool m_enableFilter;

    typedef void (Forwarder::*CommandHandler)(const QByteArray &, const QByteArray &, const QByteArray &);
    void directRequest(const QByteArray &method, const QByteArray &rawUrl, const QByteArray &protocol);
    void setupTunnel(const QByteArray &, const QByteArray &rawUrl, const QByteArray &protocol);
    void setFilter(const QByteArray &, const QByteArray &status, const QByteArray &);
    static QMap<QByteArray, CommandHandler> m_handlers;
};

#endif // FORWARDER_H
