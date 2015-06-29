#-------------------------------------------------
#
# Project created by QtCreator 2015-04-24T21:03:33
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = uvpp
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

HEADERS += \
    uvpp.h \
    loop.h \
    handle.h\
    stream.h\
    tcp.h \
    thread.h \
    loopex.h

INCLUDEPATH += $$PWD/../
LIBS += -L$$PWD/.. -llibuv/lib/libuv -lws2_32 -ladvapi32 -lpsapi -liphlpapi
