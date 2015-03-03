#-------------------------------------------------
#
# Project created by QtCreator 2015-02-26T23:35:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtProjectTest
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    socket.cpp \
    tcpconnection.cpp \
    eventloop.cpp \
    tcpserver.cpp \
    echoserver.cpp \
    chargenserver.cpp

HEADERS  += widget.h \
    socket.h \
    tcpconnection.h \
    Callbacks.h \
    eventloop.h \
    tcpserver.h \
    echoserver.h \
    chargenserver.h

FORMS    += widget.ui


DEFINES += WINVER=0x0600 _WIN32_WINNT=0x0600
LIBS += -lws2_32 -ladvapi32 -lpsapi -liphlpapi -l$$PWD/../libuv
INCLUDEPATH += $$PWD/../include
