INCLUDEPATH += $$PWD


DEFINES += WINVER=0x0600 _WIN32_WINNT=0x0600
LIBS += -lws2_32 -ladvapi32 -lpsapi -liphlpapi -luserenv

CONFIG(debug, debug|release){
    LIBS += -l$$PWD/libuv/lib/qlibuv_d
}
else{
    LIBS += -l$$PWD/libuv/lib/qlibuv
}

