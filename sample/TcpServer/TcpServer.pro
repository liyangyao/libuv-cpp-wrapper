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


SOURCES += main.cpp \
    httpserver.cpp \
    http_parser.c

include(..\..\libuv\libuv.pri)
INCLUDEPATH += ..\..\uvplusplus

HEADERS += \
    libuvpp.h \
    tcpserver.h \
    httpserver.h \
    HttpParser.h \
    Singleton.h\
    http_parser.h \
    http/Request.h \
    http/Respon.h
