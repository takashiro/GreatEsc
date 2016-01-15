#include "server.h"
#include "forwarder.h"

#include <QFile>
#include <QTextStream>
#include <QString>
#include <QStringList>

Server::Server(QObject *parent)
    : QTcpServer(parent)
{
    QFile accountFile("account.list");
    if (accountFile.exists() && accountFile.open(QFile::ReadOnly)) {
        QTextStream stream(&accountFile);
        while (!stream.atEnd()) {
            QByteArray account, password;
            stream >> account >> password;
            if (account.isEmpty() || password.isEmpty())
                continue;

            m_accounts[account] = password;
        }
    }

    connect(this, &Server::newConnection, this, &Server::handleNewConnection);
}

bool Server::login(const QByteArray &account, const QByteArray &password) const
{
    return m_accounts.contains(account) && m_accounts.value(account) == password;
}

void Server::handleNewConnection()
{
    while (hasPendingConnections()) {
        QTcpSocket *socket = nextPendingConnection();
        Forwarder *forwarder = new Forwarder(socket, this);
        if (!m_config.requireAccount)
            forwarder->setLoggedIn(true);
    }
}
