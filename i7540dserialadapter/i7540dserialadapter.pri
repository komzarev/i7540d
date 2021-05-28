include($$PWD/../../dep/qkeepalivetcpsocket/qkeepalivetcpsocket.pri)

QT -= gui
QT += network serialport

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/serialconfiguration.hpp \
    $$PWD/i7540dserialadapter_global.hpp \
    $$PWD/serialinterface.hpp \
    $$PWD/serialcontroller.hpp \
    $$PWD/i7540dserialadapter.hpp

SOURCES += \
    $$PWD/i7540dserialadapter.cpp \
    $$PWD/serialconfiguration.cpp \
    $$PWD/serialcontroller.cpp \
    $$PWD/serialinterface.cpp
