#include <QGuiApplication>
#ifndef GESC_CONSOLE
#include <QQmlApplicationEngine>
#endif

#include "server.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

#ifndef GESC_CONSOLE
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
#endif

    Server server;
    server.listen(QHostAddress::Any, 5526);

    return app.exec();
}
