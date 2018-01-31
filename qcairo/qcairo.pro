TEMPLATE = app

CAIROINCL = $$system(pkg-config cairo --cflags)
CAIROLIBS = $$system(pkg-config cairo --libs)

QMAKE_CXXFLAGS += $$CAIROINCL
LIBS += $$CAIROLIBS

# Input
HEADERS += qcairowidget.h  mainwindow.h qkapow.h
SOURCES += main.cpp mainwindow.cpp qcairowidget.cpp qkapow.cpp
FORMS += controlwidgetbase.ui

OBJECTS_DIR     = .tmp
MOC_DIR         = .tmp
