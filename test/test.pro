#-------------------------------------------------
#
# Project created by QtCreator 2015-04-19T16:12:56
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
DESTDIR = $$PWD/../build-test-bin

SOURCES += main.cpp\
    eventloop.cpp \
    timer.cpp

HEADERS += eventloop.h \
    timer.h \
    Handle.h

DEFINES += WINVER=0x0600 _WIN32_WINNT=0x0600
LIBS += -lws2_32 -ladvapi32 -lpsapi -liphlpapi
# -L$$PWD/..
LIBS += -lD:\\workspace\\project\\libuv-cpp-wrapper\\libuv\\lib\\libuv
INCLUDEPATH += $$PWD/..
