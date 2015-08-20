#-------------------------------------------------
#
# Project created by QtCreator 2015-04-18T13:08:52
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = BotanTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

INCLUDEPATH += $$PWD/../../../../thirdparty
LIBS += -L$$PWD/../../../../thirdparty
LIBS += -lbotan/lib/botan -ladvapi32 -luser32
