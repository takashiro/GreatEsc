#ifdef GESC_CONSOLE
#include <QCoreApplication>
#else
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
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

    Server server;
    Config &config = server.config();
    server.listen(QHostAddress::Any, config.localPort);

#ifndef GESC_CONSOLE
    QQmlApplicationEngine engine;
    QQmlContext *context = engine.rootContext();
    context->setContextProperty("Config", &server.config());
    engine.load(QUrl(QStringLiteral("ui/main.qml")));
#endif

    return app.exec();
}
