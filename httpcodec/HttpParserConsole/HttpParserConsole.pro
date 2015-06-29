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

INCLUDEPATH += D:\workspace\Here\uvpp\libuv-cpp-wrapper\libuv\include

LIBS += -lD:\workspace\Here\uvpp\libuv-cpp-wrapper\libuv\lib\libuv

DEFINES += WINVER=0x0600 _WIN32_WINNT=0x0600
LIBS += -lws2_32 -ladvapi32 -lpsapi -liphlpapi
