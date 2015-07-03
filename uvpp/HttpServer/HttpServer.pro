#-------------------------------------------------
#
# Project created by QtCreator 2015-07-03T09:30:49
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = HttpServer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    httpserver.cpp \
    respon.cpp

HEADERS += \
    httpserver.h \
    respon.h


include(D:\workspace\Here\uvpp\libuv-cpp-wrapper\libuv\libuv.pri)
INCLUDEPATH += D:\workspace\Here\uvpp\libuv-cpp-wrapper\uvpp\net
