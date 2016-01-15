TEMPLATE = app
CONFIG += c++11

QT += network
contains(DEFINES, GESC_CONSOLE): QT -= gui
else: QT += qml quick

SOURCES += main.cpp \
    server.cpp \
    forwarder.cpp

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    server.h \
    forwarder.h

DISTFILES += \
    ui/main.qml
