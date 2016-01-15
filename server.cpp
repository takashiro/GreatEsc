#include "server.h"
#include "forwarder.h"

Server::Server(QObject *parent)
    : QTcpServer(parent)
{
    connect(this, &Server::newConnection, this, &Server::handleNewConnection);
}

void Server::handleNewConnection()
{
    while (hasPendingConnections()) {
        QTcpSocket *socket = nextPendingConnection();
        new Forwarder(socket, this);
    }
}
