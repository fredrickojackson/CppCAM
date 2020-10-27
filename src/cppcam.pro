TEMPLATE = app
INCLUDEPATH += . Gui Geometry Importers Cutters PathGenerators PathProcessors Exporters
QT += gui opengl
CONFIG += console debug
QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-unused-function

unix:{
TARGET = ../cppcam
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
SOURCES += Gui/dlgholecut.cpp
SOURCES += Gui/dlgradialcutter.cpp
SOURCES += Gui/dlgroundcutter.cpp
SOURCES += Gui/rectcutdialog.cpp
SOURCES += Gui/dlgtoolsize.cpp
SOURCES += cppcam.cpp
SOURCES += Gui/rackcutdlg.cpp
SOURCES += Gui/dlggcodeprop.cpp
SOURCES += Gui/MainWindow.cpp
SOURCES += Gui/GLWidget.cpp
SOURCES += Gui/PreferencesDialog.cpp
SOURCES += Gui/StockModelDialog.cpp
SOURCES += Gui/ResizeModelDialog.cpp
SOURCES += Gui/AlignModelDialog.cpp
SOURCES += Gui/RotateModelDialog.cpp
SOURCES += Geometry/Geometry.cpp
SOURCES += Geometry/Boxed.cpp
SOURCES += Geometry/Stock.cpp
SOURCES += Geometry/Model.cpp
SOURCES += Geometry/Path.cpp
SOURCES += Geometry/HeightField.cpp
SOURCES += Geometry/TestModel.cpp
SOURCES += Utils/polynomials.cpp
SOURCES += Importers/STLImporter.cpp
SOURCES += Cutters/Cutter.cpp
SOURCES += Cutters/BaseCutter.cpp
SOURCES += Cutters/CylindricalCutter.cpp
SOURCES += Cutters/SphericalCutter.cpp
SOURCES += PathGenerators/DropCutter.cpp
SOURCES += PathGenerators/SimpleCutter.cpp
SOURCES += PathProcessors/PathProcessors.cpp
SOURCES += Exporters/GCodeExporter.cpp
SOURCES += Exporters/STLExporter.cpp
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
    Gui/dlgholecut.h \
    Gui/dlgradialcutter.h \
    Gui/dlgroundcutter.h \
    Gui/rectcutdialog.h \
    Gui/dlgtoolsize.h \
    Gui/rackcutdlg.h \
    Gui/dlggcodeprop.h \
    runs.h
HEADERS += Gui/MainWindow.h Gui/GLWidget.h
HEADERS += Gui/PreferencesDialog.h
HEADERS += Gui/StockModelDialog.h 
HEADERS += Gui/ResizeModelDialog.h Gui/AlignModelDialog.h Gui/RotateModelDialog.h
HEADERS +=

FORMS += Gui/MainWindow.ui Gui/AboutDialog.ui \
    Gui/dlgholecut.ui \
    Gui/dlgradialcutter.ui \
    Gui/dlgroundcutter.ui \
    Gui/rectcutdialog.ui \
    Gui/dlgtoolsize.ui \
    Gui/rackcutdlg.ui \
    Gui/dlggcodeprop.ui
FORMS += Gui/PreferencesDialog.ui Gui/StockModelDialog.ui 
FORMS += Gui/ResizeModelDialog.ui Gui/AlignModelDialog.ui Gui/RotateModelDialog.ui 
FORMS +=

RESOURCES += Gui/pycam.qrc
