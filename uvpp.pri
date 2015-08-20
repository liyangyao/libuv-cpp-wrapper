INCLUDEPATH += $$PWD
LIBS += -l$$PWD/lib/libuv

DEFINES += WINVER=0x0600 _WIN32_WINNT=0x0600
LIBS += -lws2_32 -ladvapi32 -lpsapi -liphlpapi

