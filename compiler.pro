TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -masm=intel

SOURCES += main.cpp \
    compiler.cpp \
    function.cpp \
    bytearray.cpp \
    register.cpp

HEADERS += \
    compiler.h \
    common.h \
    function.h \
    bytearray.h \
    register.h
