#-------------------------------------------------
#
# Project created by QtCreator 2015-08-22T16:44:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TwoLoop
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h

FORMS    += widget.ui

#import libuv
include($$PWD/../../uvpp.pri)
