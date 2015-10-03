#-------------------------------------------------
#
# Project created by QtCreator 2015-09-30T22:40:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pipe_sample
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h \
    PipeServer.h

FORMS    += widget.ui

include($$PWD/../../uvpp.pri)
