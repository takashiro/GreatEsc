#ifndef FORWARDER_H
#define FORWARDER_H

#include <QObject>
#include <QString>
#include <QMap>

class QTcpSocket;

class Forwarder : public QObject
{
public:
    Forwarder(QTcpSocket *client, QObject *parent = nullptr);

    QTcpSocket *proxy() const { return m_proxy; }
    void setProxy(QTcpSocket *proxy);

protected:
    void handleRequest();

    static bool isBlocked(const QString &domain);
    static void filterRequest(QByteArray &data, bool forward);

    void forwardRequest();
    void forwardResponse();

    QTcpSocket *m_client;
    QTcpSocket *m_proxy;
    QTcpSocket *m_tunnel;
};

#endif // FORWARDER_H
