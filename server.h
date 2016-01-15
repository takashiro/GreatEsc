#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>

class Server : public QTcpServer
{
public:
    Server(QObject *parent = nullptr);

protected:
    void handleNewConnection();
};

#endif // SERVER_H
