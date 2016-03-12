TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

for (var, $$list($$fromfile(../compiler/compiler.pro, SOURCES))) {
    if (!equals(var, main.cpp)) {
        LIBS += ../compiler/release/$$replace(var, .cpp, .o)
    }
}

LIBS += -L../../unit/release -lunit

DEPENDPATH += \
    ../compiler

INCLUDEPATH += \
    ../compiler \
    ../../unit

SOURCES += \
    main.cpp

HEADERS +=
