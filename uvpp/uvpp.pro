#-------------------------------------------------
#
# Project created by QtCreator 2015-08-17T21:12:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = uvpp
TEMPLATE = app


SOURCES += main.cpp\
        mainform.cpp

HEADERS  += mainform.h \
    uvpp.h \
    uvpp_define.h \
    uvpp_loop.h \
    uvpp_handle.h \
    uvpp_stream.h

FORMS    += mainform.ui

include(..\\libuv\\libuv.pri)
