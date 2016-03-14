TEMPLATE = lib
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -masm=intel

CONFIG += staticlib

SOURCES += \
    bytearray.cpp \
    common.cpp \
    compiler.cpp \
    function.cpp \
    memoryreference.cpp

HEADERS += \
    bytearray.h \
    common.h \
    compiler.h \
    function.h \
    memoryreference.h
