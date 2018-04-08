/* -*- coding: utf-8 -*-

Copyright 2014 Lode Leroy

This file is part of CppCAM.

CppCAM is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

CppCAM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with CppCAM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QtGui>
#include "GLWidget.h"

#include "ui_MainWindow.h"

class PreferencesDialog;

class MainWindow : 
    public QMainWindow, private Ui::MainWindow 
{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();
    GLWidget* theGLWidget;
    QString p_modelfilename;
    double p_mminx;
    double p_mminy;
    double p_mminz;
    double p_CutterSize;
    QString p_CutterType;
    double p_sminx;
    double p_sminy;
    double p_sminz;
    double p_smaxx;
    double p_smaxy;
    double p_smaxz;
    long p_runLines;
    double p_runStepover;
    long p_runPoints;
    double p_runResolution;
    int p_runLayers;
    double p_runStepdown;
    bool p_runRotate;
    double p_runRotStep;
    bool p_runDirectionx;
    void on_actionTestTriangle_triggered();
    void on_actionTestModel_triggered();
    void on_actionTestPath_triggered();

    void on_actionTestStockModel_triggered();


    void on_actionDrop_triggered();
    void on_actionPush_triggered();



    void on_actionAbout_triggered();

private slots:
    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionImport_Model_triggered();



    void on_actionPreferences_triggered();

    void on_actionExit_triggered();

    void on_actionEdit_Stock_Dimensions_triggered();

    void on_actionTool_Size_triggered();

    void on_actionResize_Model_triggered();

    void on_actionAlign_Model_to_Stock_triggered();

    void on_actionRotate_Model_triggered();

    void on_actionExport_Model_triggered();

    void on_actionRun_triggered();

    void on_actionExport_GCode_triggered();

    void on_actionResetView_triggered();

    void on_actionFrontView_triggered();

    void on_actionShowCutter_triggered();

    void on_actionShowStock_triggered();

    void on_actionShowModel_triggered();

    void on_actionShowModelBoundingBox_triggered();

    void on_actionShowNormals_triggered();

    void on_actionShowPath_triggered();

    void on_actionShowHeightField_triggered();

    void on_actionheight_field_triggered();

    void on_actionZoom_in_triggered();

    void on_actionZoom_Out_triggered();

    void on_actiondrop_triggered();

    void on_actionpush_triggered();

    void on_actionRectCut_triggered();

private:
    void clearPath();
    void clearHeightfield();
    void clearModelAndPath();

    PreferencesDialog* m_PreferencesDialog;
};
