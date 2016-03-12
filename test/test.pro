TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -L../compiler/release -lcompiler
LIBS += -L../../unit/release -lunit

DEPENDPATH += \
    ../compiler \
    ../../unit

INCLUDEPATH += \
    ../compiler \
    ../../unit

SOURCES += \
    main.cpp \
    movtest.cpp \
    addtest.cpp

HEADERS += \
    movtest.h \
    addtest.h
