#-------------------------------------------------
#
# Project created by QtCreator 2015-08-11T09:42:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = shadowsocks-client
TEMPLATE = app


SOURCES += main.cpp\
        mainform.cpp\
        cipher.cpp\
    encryptor.cpp

HEADERS  += mainform.h\
    libuvpp.h\
    tcpserver.h\
    cipher.h\
    encryptor.h


FORMS    += mainform.ui

DESTDIR = $$PWD/bin
include($$PWD/../libuv/libuv.pri)

INCLUDEPATH += $$PWD/include
LIBS += -L$$PWD/include
LIBS += -lbotan/lib/botan -ladvapi32 -luser32
DEFINES += NOMINMAX

QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
LIBS += -ldbghelp
