TEMPLATE = app
INCLUDEPATH += . Gui Geometry Importers Cutters PathGenerators PathProcessors Exporters
QT += gui opengl
CONFIG += console debug
#QMAKE_CXXFLAGS += -O2

unix:{
TARGET = cppcam
LIBS += -lGLU
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
RCC_DIR = build
}

windows:{
TARGET = ../../cppcam
LIBS += -lglu32
OBJECTS_DIR = buildwin32
MOC_DIR = buildwin32
UI_DIR = buildwin32
RCC_DIR = buildwin32
}

# Input
SOURCES += \
    Gui/SimpleCutterDialog.cpp \
    Gui/dlgroundcutter.cpp \
    Gui/rectcutdialog.cpp \
    Gui/dlgtoolsize.cpp \
    cppcam.cpp

SOURCES += Gui/MainWindow.cpp Gui/GLWidget.cpp 
SOURCES += Gui/PreferencesDialog.cpp Gui/StockModelDialog.cpp
SOURCES += Gui/ResizeModelDialog.cpp Gui/AlignModelDialog.cpp Gui/RotateModelDialog.cpp
SOURCES += Geometry/Geometry.cpp Geometry/Boxed.cpp Geometry/Stock.cpp Geometry/Model.cpp 
SOURCES += Geometry/Path.cpp Geometry/HeightField.cpp
SOURCES += Geometry/TestModel.cpp
SOURCES += Utils/polynomials.cpp
SOURCES += Importers/STLImporter.cpp
SOURCES += Cutters/Cutter.cpp Cutters/BaseCutter.cpp 
SOURCES += Cutters/CylindricalCutter.cpp Cutters/SphericalCutter.cpp
SOURCES += PathGenerators/DropCutter.cpp PathGenerators/SimpleCutter.cpp
SOURCES += PathProcessors/PathProcessors.cpp
SOURCES += Exporters/GCodeExporter.cpp Exporters/STLExporter.cpp
SOURCES += Tests/TestRunner.cpp

HEADERS += Gui/AboutDialog.h \
    Geometry/Point.h \
    Geometry/Triangle.h \
    Geometry/HeightField.h \
    Geometry/Model.h \
    Geometry/Boxed.h \
    Geometry/Geometry.h \
    Geometry/Path.h \
    Geometry/Stock.h \
    Geometry/TestModel.h \
    Geometry/Edge.h \
    Geometry/intersection.h \
    Gui/dlgroundcutter.h \
    Gui/rectcutdialog.h \
    Gui/dlgtoolsize.h
HEADERS += Gui/MainWindow.h Gui/GLWidget.h
HEADERS += Gui/PreferencesDialog.h
HEADERS += Gui/StockModelDialog.h 
HEADERS += Gui/ResizeModelDialog.h Gui/AlignModelDialog.h Gui/RotateModelDialog.h
HEADERS += Gui/SimpleCutterDialog.h

FORMS += Gui/MainWindow.ui Gui/AboutDialog.ui \
    Gui/dlgroundcutter.ui \
    Gui/rectcutdialog.ui \
    Gui/dlgtoolsize.ui
FORMS += Gui/PreferencesDialog.ui Gui/StockModelDialog.ui 
FORMS += Gui/ResizeModelDialog.ui Gui/AlignModelDialog.ui Gui/RotateModelDialog.ui 
FORMS += Gui/SimpleCutterDialog.ui

RESOURCES += Gui/pycam.qrc
