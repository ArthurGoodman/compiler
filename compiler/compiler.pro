TEMPLATE = lib
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

CONFIG += staticlib

SOURCES += \
    bytearray.cpp \
    common.cpp \
    compiler.cpp \
    function.cpp \
    memref.cpp

HEADERS += \
    bytearray.h \
    common.h \
    compiler.h \
    function.h \
    memref.h
