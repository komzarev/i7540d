include($$PWD/../../dep/qmake/common.pri)

QT -= gui
QT += network

CONFIG += c++11

INCLUDEPATH += $$PWD

HEADERS += $$PWD/keepalivetcpsocket.hpp

SOURCES += $$PWD/keepalivetcpsocket.cpp

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

DISTFILES += qkeepalivetcpsocket.pri
