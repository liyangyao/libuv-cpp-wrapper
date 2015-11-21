QT += core
QT -= gui

TARGET = jsonrpc_sample
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    myservice.cpp

HEADERS += \
    myservice.h

