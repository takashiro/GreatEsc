#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "server.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    Server server;
    server.listen(QHostAddress::Any, 5526);

    return app.exec();
}
