#ifdef GESC_CONSOLE
#include <QCoreApplication>
#else
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#endif

#include "server.h"

int main(int argc, char *argv[])
{
#ifdef GESC_CONSOLE
    QCoreApplication app(argc, argv);
#else
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
#endif

    Server server;
    server.listen(QHostAddress::Any, 5526);

    return app.exec();
}
