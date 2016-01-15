#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QByteArray>
#include <QObject>

class Config : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString server READ server WRITE setServer NOTIFY serverChanged)
    Q_PROPERTY(QString account READ account WRITE setAccount NOTIFY accountChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)

public:
    QString remoteName;
    ushort remotePort;
    QByteArray userName;
    QByteArray userPassword;

    ushort localPort;
    bool requireAccount;
    bool isOutsider;

    Config();
    ~Config();

    QString server() const;
    void setServer(const QString &server);

    QString account() const;
    void setAccount(const QString &account);

    QString password() const;
    void setPassword(const QString &password);

signals:
    void serverChanged();
    void accountChanged();
    void passwordChanged();
};

#endif // CONFIG_H
