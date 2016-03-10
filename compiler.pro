TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXX += -masm=intel

SOURCES += main.cpp \
    vm.cpp \
    compiler.cpp \
    function.cpp \
    bytearray.cpp

HEADERS += \
    vm.h \
    compiler.h \
    common.h \
    function.h \
    bytearray.h
