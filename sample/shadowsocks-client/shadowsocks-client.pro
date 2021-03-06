#-------------------------------------------------
#
# Project created by QtCreator 2015-08-11T09:42:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = shadowsocks-client
TEMPLATE = app
DESTDIR = $$PWD/bin

SOURCES += main.cpp\
        mainform.cpp\
    li_utility.cpp

HEADERS  += mainform.h\
    botan_wrapper.h \
    Buffer.h\
    li_utility.h

FORMS    += mainform.ui

#botan
DEFINES += NOMINMAX
INCLUDEPATH += $$PWD/include
CONFIG(debug, debug|release){
    LIBS += -l$$PWD/include/botan/lib/botan_d
}
else{
    LIBS += -l$$PWD/include/botan/lib/botan
}
#uvpp
include($$PWD/../../uvpp.pri)

RC_FILE = app.rc
RESOURCES += app.qrc
LIBS += -luser32
QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

DEFINES += CHECK_UV_REQ


