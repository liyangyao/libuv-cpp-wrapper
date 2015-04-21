#-------------------------------------------------
#
# Project created by QtCreator 2015-04-18T15:25:36
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = LibuvTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

INCLUDEPATH += $$PWD/../../..
LIBS += -L$$PWD/../../..

DEFINES += WINVER=0x0600 _WIN32_WINNT=0x0600
LIBS += -llibuv/lib/libuv -lws2_32 -ladvapi32 -lpsapi -liphlpapi
