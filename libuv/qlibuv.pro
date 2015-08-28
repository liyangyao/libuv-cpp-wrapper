QT       -= core gui

CONFIG(debug, debug|release){
    TARGET = qlibuv_d
}
else{
    TARGET = qlibuv
}
TEMPLATE = lib
CONFIG += staticlib
DESTDIR = $$PWD/lib

INCLUDEPATH += $$PWD

HEADERS += $$PWD/src/heap-inl.h
HEADERS += $$PWD/src/queue.h
HEADERS += $$PWD/src/uv-common.h
SOURCES += $$PWD/src/fs-poll.c
SOURCES += $$PWD/src/inet.c
SOURCES += $$PWD/src/threadpool.c
SOURCES += $$PWD/src/uv-common.c
SOURCES += $$PWD/src/version.c

win32{
    HEADERS += $$PWD/src/win/atomicops-inl.h
    HEADERS += $$PWD/src/win/handle-inl.h
    HEADERS += $$PWD/src/win/internal.h
    HEADERS += $$PWD/src/win/req-inl.h
    HEADERS += $$PWD/src/win/stream-inl.h
    HEADERS += $$PWD/src/win/winapi.h
    HEADERS += $$PWD/src/win/winsock.h

    SOURCES += $$PWD/src/win/async.c
    SOURCES += $$PWD/src/win/core.c
    SOURCES += $$PWD/src/win/dl.c
    SOURCES += $$PWD/src/win/error.c
    SOURCES += $$PWD/src/win/fs-event.c
    SOURCES += $$PWD/src/win/fs.c
    SOURCES += $$PWD/src/win/getaddrinfo.c
    SOURCES += $$PWD/src/win/getnameinfo.c
    SOURCES += $$PWD/src/win/handle.c
    SOURCES += $$PWD/src/win/loop-watcher.c
    SOURCES += $$PWD/src/win/pipe.c
    SOURCES += $$PWD/src/win/poll.c
    SOURCES += $$PWD/src/win/process-stdio.c
    SOURCES += $$PWD/src/win/process.c
    SOURCES += $$PWD/src/win/req.c
    SOURCES += $$PWD/src/win/signal.c
    SOURCES += $$PWD/src/win/stream.c
    SOURCES += $$PWD/src/win/tcp.c
    SOURCES += $$PWD/src/win/thread.c
    SOURCES += $$PWD/src/win/timer.c
    SOURCES += $$PWD/src/win/tty.c
    SOURCES += $$PWD/src/win/udp.c
    SOURCES += $$PWD/src/win/util.c
    SOURCES += $$PWD/src/win/winapi.c
    SOURCES += $$PWD/src/win/winsock.c
}

unix {
    target.path = /usr/lib
    INSTALLS += target

    HEADERS += $$PWD/src/unix/atomic-ops.h
    HEADERS += $$PWD/src/unix/internal.h
    HEADERS += $$PWD/src/unix/linux-syscalls.h
    HEADERS += $$PWD/src/unix/spinlock.h

    SOURCES += $$PWD/src/unix/aix.c
    SOURCES += $$PWD/src/unix/android-ifaddrs.c
    SOURCES += $$PWD/src/unix/async.c
    SOURCES += $$PWD/src/unix/core.c
    SOURCES += $$PWD/src/unix/darwin-proctitle.c
    SOURCES += $$PWD/src/unix/darwin.c
    SOURCES += $$PWD/src/unix/dl.c
    SOURCES += $$PWD/src/unix/freebsd.c
    SOURCES += $$PWD/src/unix/fs.c
    SOURCES += $$PWD/src/unix/fsevents.c
    SOURCES += $$PWD/src/unix/getaddrinfo.c
    SOURCES += $$PWD/src/unix/getnameinfo.c
    SOURCES += $$PWD/src/unix/kqueue.c
    SOURCES += $$PWD/src/unix/linux-core.c
    SOURCES += $$PWD/src/unix/linux-inotify.c
    SOURCES += $$PWD/src/unix/linux-syscalls.c
    SOURCES += $$PWD/src/unix/loop-watcher.c
    SOURCES += $$PWD/src/unix/loop.c
    SOURCES += $$PWD/src/unix/netbsd.c
    SOURCES += $$PWD/src/unix/openbsd.c
    SOURCES += $$PWD/src/unix/pipe.c
    SOURCES += $$PWD/src/unix/poll.c
    SOURCES += $$PWD/src/unix/process.c
    SOURCES += $$PWD/src/unix/proctitle.c
    SOURCES += $$PWD/src/unix/pthread-fixes.c
    SOURCES += $$PWD/src/unix/signal.c
    SOURCES += $$PWD/src/unix/stream.c
    SOURCES += $$PWD/src/unix/sunos.c
    SOURCES += $$PWD/src/unix/tcp.c
    SOURCES += $$PWD/src/unix/thread.c
    SOURCES += $$PWD/src/unix/timer.c
    SOURCES += $$PWD/src/unix/tty.c
    SOURCES += $$PWD/src/unix/udp.c
}




