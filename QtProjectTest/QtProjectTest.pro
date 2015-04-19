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
    $$PWD/../libuv_wrapper/socket.cpp \
    $$PWD/../libuv_wrapper/tcpconnection.cpp \
    $$PWD/../libuv_wrapper/eventloop.cpp \
    $$PWD/../libuv_wrapper/tcpserver.cpp \
    $$PWD/../libuv_wrapper/echoserver.cpp \
    $$PWD/../libuv_wrapper/chargenserver.cpp

HEADERS  += widget.h \
    $$PWD/../libuv_wrapper/socket.h \
    $$PWD/../libuv_wrapper/tcpconnection.h \
    $$PWD/../libuv_wrapper/Callbacks.h \
    $$PWD/../libuv_wrapper/eventloop.h \
    $$PWD/../libuv_wrapper/tcpserver.h \
    $$PWD/../libuv_wrapper/echoserver.h \
    $$PWD/../libuv_wrapper/chargenserver.h

FORMS    += widget.ui


DEFINES += WINVER=0x0600 _WIN32_WINNT=0x0600
LIBS += -lws2_32 -ladvapi32 -lpsapi -liphlpapi -L$$PWD/.. -llibuv/lib/libuv
INCLUDEPATH += $$PWD/../libuv/include
