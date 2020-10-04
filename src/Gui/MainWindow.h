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
#include <QList>
#include "GLWidget.h"

#include "ui_MainWindow.h"

#include "Cutter.h"
#include "Stock.h"
#include "Model.h"
#include "runs.h"

class PreferencesDialog;

class MainWindow : 
    public QMainWindow, private Ui::MainWindow 
{
    Q_OBJECT
public:

    MainWindow();
    ~MainWindow();

    void logit(QString qstr);
    GLWidget* theGLWidget;
    QString p_modelfilename;
    QString p_projectfilename;
    long p_runLines;
    double p_runStepover;
    long p_runPoints;
    double p_runResolution;
    int p_runLayers;
    double p_runStepdown;
    bool p_runRotate;
    double p_runRotateDegrees;
    double p_runRotateStep;
    unsigned int p_runDirection;
    double p_safez;
    double p_FeedSpeed;
    double p_PlungeSpeed;
    int p_useLine;
    bool p_rectcut;
    int p_smooth;
    double p_comp;
    bool bEnableArcs;
    QTimer *qtimer;
    int p_runs;
    QList<r_run> p_qlruns;
    bool radialcut(r_run myrun);
    void selectRun(int iRun);
    Point p_dir;
    r_run ttrun;
    void hole_cut();
    int iRunInd;
    void upd();

private slots:
    void resizeEvent(QResizeEvent* event);
    void mouseMoveEvent(QMouseEvent *ev);
    void mytimeout();

    void on_actionAbout_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionImport_Model_triggered();

    void on_actionPreferences_triggered();

    void on_actionExit_triggered();

    void on_actionEdit_Stock_Dimensions_triggered();

    void on_actionTool_Size_triggered();

    void on_actionResize_Model_triggered();

    void on_actionRotate_Model_triggered();

    void on_actionExport_Model_triggered();

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

    void on_actionRack_Cut_triggered();

    void on_actionHole_Cut_triggered();

    void on_actionClear_Path_triggered();

    void on_actionAlign_Model_triggered();

    void on_actionRadial_Cut_triggered();

    void on_pbAdd_clicked();

    void on_pbRemove_clicked();

    void on_pbRun_clicked();

    void on_pbClr_clicked();

    void on_pbUp_clicked();

    void on_pbDown_clicked();

    void on_pbRun1_clicked();

private:
    void clearPath();
    void clearHeightfield();
    void clearModelAndPath();
    PreferencesDialog* m_PreferencesDialog;
    void sim();

};
