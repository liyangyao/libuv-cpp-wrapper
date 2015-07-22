#-------------------------------------------------
#
# Project created by QtCreator 2015-07-22T21:51:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HttpClientTool
TEMPLATE = app


SOURCES += main.cpp\
        manform.cpp

HEADERS  += manform.h

FORMS    += manform.ui

INCLUDEPATH += $$PWD/../..
LIBS += -L$$PWD/../..

DEFINES += CURL_STATICLIB
LIBS += -lws2_32 -lwinmm -lwldap32 -lAdvapi32 -llibcurl/static_lib/libcurl
