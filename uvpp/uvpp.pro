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
    uvpp_stream.h \
    uvpp_timer.h \
    uvpp_thread.h \
    uvpp_async.h \
    uvpp_tcp.h \
    uvpp_pipe.h \
    uvpp_asyncfunctor.h \
    uvpp_connection.h \
    uvpp_tcpserver.h \
    uvpp_idle.h

FORMS    += mainform.ui

include(..\\libuv\\libuv.pri)
