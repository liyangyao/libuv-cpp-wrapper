#-------------------------------------------------
#
# Project created by QtCreator 2015-09-14T10:11:34
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = JsonRPC
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    service.cpp

HEADERS += \
    service.h \
    jsonrpc/message.h \
    jsonrpc/result.h \
    jsonrpc/serviceprovider.h
