TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -L../compiler/release -lcompiler
LIBS += -L../../unit/release -lunit

PRE_TARGETDEPS += ../compiler/release/libcompiler.a

INCLUDEPATH += \
    ../compiler \
    ../../unit

SOURCES += \
    main.cpp

HEADERS +=
