#-------------------------------------------------
#
# Project created by QtCreator 2015-07-18T14:49:18
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = TcpServer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

include(D:\workspace\project\libuv-cpp-wrapper\libuv\libuv.pri)
INCLUDEPATH += D:\workspace\project\libuv-cpp-wrapper

HEADERS += \
    libuvpp.h
