include($$PWD/../../dep/qmake/common.pri)
include($$PWD/../../dep/qkeepalivetcpsocket/qkeepalivetcpsocket.pri)

TARGET = i7540can
TEMPLATE = lib
CONFIG += plugin

QT = core serialbus network

HEADERS += \
    i7540canbackend.hpp \
    canconfiguration.hpp\
    cancontroller.hpp \
    caninterface.hpp \
    canserializer.hpp

SOURCES += \
    i7540canbackend.cpp \
    canconfiguration.cpp \
    cancontroller.cpp \
    caninterface.cpp \
    canserializer.cpp \
    main.cpp

DISTFILES = plugin.json

PLUGIN_TYPE = canbus
PLUGIN_EXTENDS = serialbus
PLUGIN_CLASS_NAME = I7540CanBusPlugin

exists($$PWD/../../dep/qmake/version.pri) {
    include($$PWD/../../dep/qmake/version.pri)
}
