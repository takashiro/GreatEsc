#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>

class Server : public QTcpServer
{
public:
    Server(QObject *parent = nullptr);

    bool login(const QByteArray &account, const QByteArray &password) const;

protected:
    void handleNewConnection();

    QMap<QByteArray, QByteArray> m_accounts;
};

#endif // SERVER_H
