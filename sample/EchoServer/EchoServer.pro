#-------------------------------------------------
#
# Project created by QtCreator 2015-08-24T09:52:26
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = EchoServer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

#import libuv
include($$PWD/../../uvpp.pri)
