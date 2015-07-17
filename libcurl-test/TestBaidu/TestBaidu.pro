#-------------------------------------------------
#
# Project created by QtCreator 2015-04-07T08:56:06
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TestBaidu
TEMPLATE = app
DESTDIR = $$PWD/../build-bin

SOURCES += main.cpp\
        widget.cpp\
    qcurl.cpp

HEADERS  += widget.h\
    qcurl.h

FORMS    += widget.ui

INCLUDEPATH += D:\workspace\Here\uvpp\libuv-cpp-wrapper
LIBS += -LD:\workspace\Here\uvpp\libuv-cpp-wrapper

DEFINES += CURL_STATICLIB
LIBS += -lws2_32 -lwinmm -lwldap32 -lAdvapi32 -llibcurl/static_lib/libcurl
#LIBS += -lws2_32 -lwinmm -lwldap32 -lAdvapi32 -llibcurl/lib/libcurl




