TEMPLATE = app
CONFIG += c++11 console_only
QT += network

console_only{
    DEFINES += GESC_CONSOLE
    QT -= gui
}else{
    QT += qml quick
    RESOURCES += qml.qrc
}

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
