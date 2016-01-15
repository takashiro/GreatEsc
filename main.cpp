#ifdef GESC_CONSOLE
#include <QCoreApplication>
#else
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#endif
#include <QSettings>

#include "server.h"

int main(int argc, char *argv[])
{
#ifdef GESC_CONSOLE
    QCoreApplication app(argc, argv);
#else
    QGuiApplication app(argc, argv);
#endif
    app.setApplicationName("GreatEsc");
    app.setOrganizationName("Takashiro");
    app.setOrganizationDomain("takashiro.me");

#ifndef GESC_CONSOLE
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("ui/main.qml")));
#endif

    Server server;
    server.listen(QHostAddress::Any, 5526);

    return app.exec();
}
