#ifndef SERVER_H
#define SERVER_H

#include "config.h"

#include <QTcpServer>

class Server : public QTcpServer
{
public:
    Server(QObject *parent = nullptr);

    bool login(const QByteArray &account, const QByteArray &password) const;

    Config &config() { return m_config; }

protected:
    void handleNewConnection();

    QMap<QByteArray, QByteArray> m_accounts;
    Config m_config;
};

#endif // SERVER_H
