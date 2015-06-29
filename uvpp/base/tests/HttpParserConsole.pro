#-------------------------------------------------
#
# Project created by QtCreator 2015-06-24T16:45:56
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = HttpParserConsole
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp\
    http_parser.c

HEADERS += http_parser.h \
    BlockingQueue.h \
    HttpParser.h

include(D:\workspace\Here\uvpp\libuv-cpp-wrapper\libuv\libuv.pri)


