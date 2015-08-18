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
        mainform.cpp

HEADERS  += mainform.h\
    botan_wrapper.h \
    Buffer.h


FORMS    += mainform.ui

DESTDIR = $$PWD/bin
include($$PWD/../libuv/libuv.pri)

RC_FILE = app.rc

INCLUDEPATH += $$PWD/include
INCLUDEPATH += $$PWD/../uvpp
LIBS += -L$$PWD/include
LIBS += -ladvapi32 -luser32
DEFINES += NOMINMAX

CONFIG(debug, debug|release){
    LIBS += -lbotan/lib/botan_d
}
else{
    LIBS += -lbotan/lib/botan
}

QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
LIBS += -ldbghelp

RESOURCES += \
    app.qrc
