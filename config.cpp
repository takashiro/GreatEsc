#include "config.h"

#include <QSettings>

Config::Config()
{
    QSettings config("config.ini", QSettings::IniFormat);
    remoteName = config.value("Remote/Name", "p.takashiro.me").toString();
    remotePort = config.value("Remote/Port", 1010).toUInt();
    userName = config.value("Remote/Account").toByteArray();
    userPassword = config.value("Remote/Password").toByteArray();

    requireAccount = config.value("Local/RequireAccount", false).toBool();
    localPort = config.value("Local/Port", 1010).toUInt();
    isOutsider = config.value("Local/Outsider", false).toBool();
}

Config::~Config()
{
    QSettings config("config.ini", QSettings::IniFormat);
    config.setValue("Remote/Name", remoteName);
    config.setValue("Remote/Port", remotePort);
    config.setValue("Remote/Account", userName);
    config.setValue("Remote/Password", userPassword);

    config.setValue("Local/RequireAccount", requireAccount);
    config.setValue("Local/Port", localPort);
    config.setValue("Local/Outsider", isOutsider);
}

QString Config::server() const
{
    return QString("%1:%2").arg(remoteName).arg(remotePort);
}

void Config::setServer(const QString &server)
{
    QStringList parts = server.split(':');
    remoteName = parts.at(0);
    if (parts.length() > 1) {
        remotePort = parts.at(1).toUShort();
    } else {
        remotePort = 1010;
    }
    emit serverChanged();
}

QString Config::account() const
{
    return userName;
}

void Config::setAccount(const QString &account)
{
    userName = account.toLatin1();
    emit accountChanged();
}

QString Config::password() const
{
    return userPassword;
}

void Config::setPassword(const QString &password)
{
    userPassword = password.toLatin1();
    emit passwordChanged();
}

