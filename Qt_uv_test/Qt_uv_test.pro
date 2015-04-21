#-------------------------------------------------
#
# Project created by QtCreator 2015-04-20T20:46:08
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = Qt_uv_test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

INCLUDEPATH += $$PWD/../
LIBS += -L$$PWD/.. -llibuv/lib/libuv -lws2_32 -ladvapi32 -lpsapi -liphlpapi

HEADERS += \
    loop.h \
    uvqt.h \
    idle.h \
    timer.h \
    tcp.h \
    handle.h \
    stream.h \
    dns.h
