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
#include <QtCore>
#include <QFileDialog>
#include <QProgressBar>
#include <QMessageBox>

#include "MainWindow.h"

#include "AboutDialog.h"
#include "PreferencesDialog.h"
#include "StockModelDialog.h"
#include "ResizeModelDialog.h"
#include "AlignModelDialog.h"
#include "RotateModelDialog.h"
#include "SimpleCutterDialog.h"
#include "dlgtoolsize.h"
#include "dlgcutter.h"
#include "Stock.h"
#include "Model.h"
#include "Path.h"
#include "Cutter.h"
#include "DropCutter.h"
#include "SimpleCutter.h"
#include "TestModel.h"
#include "STLImporter.h"
#include "GCodeExporter.h"
#include "STLExporter.h"
#include "rectcutdialog.h"

extern Stock* stock;
extern Model* model;
extern Cutter* cutter;
extern std::vector<Path*> paths;
extern HeightField* heightfield;

MainWindow::MainWindow() 
{
    setupUi(this);
    theGLWidget = new GLWidget();
    setCentralWidget(theGLWidget);
    m_PreferencesDialog = new PreferencesDialog();

//    QObject::connect(actionResetView, SIGNAL(triggered()), theGLWidget, SLOT(resetView()));
    QObject::connect(actionTopView, SIGNAL(triggered()), theGLWidget, SLOT(setTopView()));
    QObject::connect(actionLeftView, SIGNAL(triggered()), theGLWidget, SLOT(setLeftView()));
    QObject::connect(actionFrontView, SIGNAL(triggered()), theGLWidget, SLOT(setFrontView()));
    QObject::connect(actionRightView, SIGNAL(triggered()), theGLWidget, SLOT(setRightView()));

//    QObject::connect(actionShowStock, SIGNAL(triggered(bool)), theGLWidget, SLOT(setShowStock(bool)));
//    QObject::connect(actionShowModel, SIGNAL(triggered(bool)), theGLWidget, SLOT(setShowModel(bool)));
//    QObject::connect(actionShowModelBoundingBox, SIGNAL(triggered(bool)), theGLWidget, SLOT(setShowModelBoundingBox(bool)));
//    QObject::connect(actionShowPath, SIGNAL(triggered(bool)), theGLWidget, SLOT(setShowPath(bool)));
//    QObject::connect(actionShowNormals, SIGNAL(triggered(bool)), theGLWidget, SLOT(setShowNormals(bool)));
//    QObject::connect(actionShowCutter, SIGNAL(triggered(bool)), theGLWidget, SLOT(setShowCutter(bool)));
//    QObject::connect(actionShowHeightField, SIGNAL(triggered(bool)), theGLWidget, SLOT(setShowHeightField(bool)));
    cutter = Cutter::CreateCylindricalCutter(1.5875, Point(0,0,0));
    stock = new Stock();
    stock->m_max_x=10.0;
    stock->m_max_y=7.0;
    stock->m_max_z=3;
    stock->m_min_x=0.0;
    stock->m_min_y=0.0;
    stock->m_min_z=0.0;

    model=new TestModel();

    //on_actionTestStockModel_triggered();
    //on_actionTool_Size_triggered();
    //model = STLImporter::ImportModel("data/SampleScene.stl");
    //on_actionTestModel_triggered();
    //on_actionDrop_triggered();

     p_mminx=model->min_x();
     p_mminy=model->min_y();
     p_mminz=model->min_z();
     p_CutterSize=1.5875;
    QString p_CutterType="Sph";
     p_sminx=stock->min_x();
     p_sminy=stock->min_y();
     p_sminz=stock->min_z();
     p_smaxx=stock->max_x();
     p_smaxy=stock->max_y();
     p_smaxz=stock->max_z();
     p_runLines=100;
     p_runStepover=0.333;
     p_runPoints=125;
     p_runResolution=0.375;
     p_runLayers=5;
     p_runStepdown=2.0;
     p_runRotate=false;
     p_runRotStep=15.0;
     p_runDirectionx=true;
}

MainWindow::~MainWindow() 
{
    delete m_PreferencesDialog;
}

void
MainWindow::on_actionAbout_triggered()
{
    AboutDialog dlg;
    dlg.exec();
}


void
MainWindow::clearPath()
{
    for(size_t i=0; i<paths.size(); i++) {
        delete paths[i];
    }
    paths.clear();
}

void
MainWindow::clearHeightfield()
{
    if (heightfield != NULL) {
        delete heightfield;
        heightfield = NULL;
    }
}

void
MainWindow::clearModelAndPath()
{
    clearPath();
    clearHeightfield();
    if (model != NULL) {
        delete model;
        model=NULL;
    }
}

void
MainWindow::on_actionTestStockModel_triggered()
{
    if (stock != NULL) {
        delete stock;
    }
    stock = new Stock();
    stock->m_min_x = 0;
    stock->m_min_y = 0;
    stock->m_min_z = 0;
    stock->m_max_x = 14;
    stock->m_max_y = 10;
    stock->m_max_z = 4;
}

void
MainWindow::on_actionTestTriangle_triggered()
{
    clearModelAndPath();

    model = new Model();
    model->add_triangle(Point(3,0,1), Point(0,3,1.5), Point(3,3,0.5));
}

void
MainWindow::on_actionTestModel_triggered()
{
    clearModelAndPath();

    model = new TestModel();
    //for(int i=0; i<2; ++i) model->subdivide(0.3);
}

void
MainWindow::on_actionTestPath_triggered()
{
    clearModelAndPath();

    Path* path = new Path();
    path->m_runs.resize(2);
    path->m_runs[0].m_points.push_back(Point(-6,-6,0));
    path->m_runs[0].m_points.push_back(Point(-5,-5,5));
    path->m_runs[0].m_points.push_back(Point(5,5,5));
    path->m_runs[0].m_points.push_back(Point(6,6,0));

    path->m_runs[1].m_points.push_back(Point(-6,-5,0));
    path->m_runs[1].m_points.push_back(Point(-5,-4,5));
    path->m_runs[1].m_points.push_back(Point(5,6,5));
    path->m_runs[1].m_points.push_back(Point(6,7,0));
    paths.push_back(path);
}

void MainWindow::on_actionOpen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Select Project File", "/home/fred", "Cam files (*.cam)");
    //p_modelfilename=filename;
    QFile gc(filename);
    if (!gc.open(QIODevice::ReadOnly)) {
        return;
    }
    QTextStream ts(&gc);
    ts >> p_modelfilename;
    model = STLImporter::ImportModel(p_modelfilename.toStdString());
    ts >> p_mminx;
    ts >> p_mminy;
    ts >> p_mminz;
    model->resize(p_mminx,1.0,p_mminy,1.0,p_mminz,1.0);
    ts >> p_CutterSize;
    ts >> p_CutterType;
    if(p_CutterType.compare("Cyl")==0){
        cutter = Cutter::CreateCylindricalCutter(p_CutterSize, Point(0,0,0));
    }else{
        cutter = Cutter::CreateSphericalCutter(p_CutterSize, Point(0,0,0));
    }
    ts >> p_sminx;
    ts >> p_sminy;
    ts >> p_sminz;
    ts >> p_smaxx;
    ts >> p_smaxy;
    ts >> p_smaxz;
    if(stock)delete stock;
    stock = new Stock();
    stock->m_min_x=p_sminx;
    stock->m_min_y=p_sminy;
    stock->m_min_z=p_sminz;
    stock->m_max_x=p_smaxx;
    stock->m_max_y=p_smaxy;
    stock->m_max_z=p_smaxz;
    ts >> p_runLines;
    ts >> p_runStepover;
    ts >> p_runPoints;
    ts >> p_runResolution;
    ts >> p_runLayers;
    ts >> p_runStepdown;
    ts >> filename;
    if(filename.compare("1")==0){
        p_runRotate=true;
    }else{
        p_runRotate=false;
    }
    ts >> p_runRotStep;
    ts >> filename;
    if(filename.compare("1")==0){
        p_runDirectionx=true;
    }else{
        p_runDirectionx=false;
    }




}

void MainWindow::on_actionSave_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this, "Select Project File", "/home/fred", "Cam files (*.cam)");
    QFile gc(filename);
    if (!gc.open(QIODevice::WriteOnly)) {
        return;
    }
    QTextStream ts(&gc);
    ts << p_modelfilename << "\n";
    ts << p_mminx << "\n";
    ts << p_mminy << "\n";
    ts << p_mminz << "\n";
    ts << p_CutterSize << "\n";
    ts << p_CutterType << "\n";
    ts << p_sminx << "\n";
    ts << p_sminy << "\n";
    ts << p_sminz << "\n";
    ts << p_smaxx << "\n";
    ts << p_smaxy << "\n";
    ts << p_smaxz << "\n";
    ts << p_runLines << "\n";
    ts << p_runStepover << "\n";
    ts << p_runPoints << "\n";
    ts << p_runResolution << "\n";
    ts << p_runLayers << "\n";
    ts << p_runStepdown << "\n";
    ts << p_runRotate << "\n";
    ts << p_runRotStep << "\n";
    ts << p_runDirectionx << "\n";


    gc.close();

}

void MainWindow::on_actionImport_Model_triggered()
{
    clearModelAndPath();

    static QString lastFile = QString::null;
    QString filename = QFileDialog::getOpenFileName(this, "Import STL File", lastFile.isNull()?"data/TestModel.stl":lastFile, "STL files (*.stl)");
    if (!filename.isEmpty()) {
        lastFile = filename;
        model = STLImporter::ImportModel(filename.toStdString());
        p_modelfilename=filename;
        p_mminx=model->min_x();
        p_mminy=model->min_y();
        p_mminz=model->min_z();
    }

}


void MainWindow::on_actionPreferences_triggered()
{
    m_PreferencesDialog->show();
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_actionEdit_Stock_Dimensions_triggered()
{
    StockModelDialog dlg(this);

    if (stock != NULL) {
        dlg.lineEdit_minx->setText(QString::number(stock->min_x()));
        dlg.lineEdit_miny->setText(QString::number(stock->min_y()));
        dlg.lineEdit_minz->setText(QString::number(stock->min_z()));

        dlg.lineEdit_dimx->setText(QString::number(stock->dim_x()));
        dlg.lineEdit_dimy->setText(QString::number(stock->dim_y()));
        dlg.lineEdit_dimz->setText(QString::number(stock->dim_z()));
    }
    dlg.exec();

    if (dlg.result() == QDialog::Accepted) {

        if (stock == NULL){
            stock = new Stock();
        }

        if(dlg.checkBoxResizeToModel->isChecked()){
            stock->m_min_x = model->min_x();
            stock->m_min_y = model->min_y();
            stock->m_min_z = model->min_z();
            stock->m_max_x = model->max_x();
            stock->m_max_y = model->max_y();
            stock->m_max_z = model->max_z();
        }else{

            stock->m_min_x = dlg.lineEdit_minx->text().toDouble();
            stock->m_min_y = dlg.lineEdit_miny->text().toDouble();
            stock->m_min_z = dlg.lineEdit_minz->text().toDouble();
            stock->m_max_x = stock->m_min_x + dlg.lineEdit_dimx->text().toDouble();
            stock->m_max_y = stock->m_min_y + dlg.lineEdit_dimy->text().toDouble();
            stock->m_max_z = stock->m_min_z + dlg.lineEdit_dimz->text().toDouble();
        }
        p_sminx=stock->min_x();
        p_sminy=stock->min_y();
        p_sminz=stock->min_z();
        p_smaxx=stock->max_x();
        p_smaxy=stock->max_y();
        p_smaxz=stock->max_z();
    }

}

void MainWindow::on_actionTool_Size_triggered()
{
    dlgToolSize dlg(this);
    dlg.exec();
    delete cutter;
    if(dlg.isSph()){
        p_CutterSize=dlg.m_ts().toDouble();
        p_CutterType="Sph";
        cutter = Cutter::CreateSphericalCutter(p_CutterSize, Point(0,0,0));
    }else{
        p_CutterSize=dlg.m_ts().toDouble();
        p_CutterType="Cyl";
        cutter = Cutter::CreateCylindricalCutter(p_CutterSize, Point(0,0,0));
    }


}


void MainWindow::on_actionResize_Model_triggered()
{
    ResizeModelDialog dlg(this);

    if (model == NULL) return;

    double m_min_x,m_min_y,m_min_z;
    double m_max_x,m_max_y,m_max_z;

    m_min_x = model->min_x();
    m_min_y = model->min_y();
    m_min_z = model->min_z();
    m_max_x = model->max_x();
    m_max_y = model->max_y();
    m_max_z = model->max_z();

    double m_dim_x = m_max_x - m_min_x;
    double m_dim_y = m_max_y - m_min_y;
    double m_dim_z = m_max_z - m_min_z;

    double s_dim_x,s_dim_y,s_dim_z;
    double s_min_x,s_min_y,s_min_z;
    if (stock != NULL) {
        s_min_x = stock->min_x();
        s_min_y = stock->min_y();
        s_min_z = stock->min_z();
        s_dim_x = stock->dim_x();
        s_dim_y = stock->dim_y();
        s_dim_z = stock->dim_z();
    } else {
        s_min_x = 0;
        s_min_y = 0;
        s_min_z = 0;
        s_dim_x = m_dim_x;
        s_dim_y = m_dim_y;
        s_dim_z = m_dim_z;
    }

    dlg.lineEdit_dimx->setText(QString::number(m_dim_x));
    dlg.lineEdit_dimy->setText(QString::number(m_dim_y));
    dlg.lineEdit_dimz->setText(QString::number(m_dim_z));
    dlg.radioButton_stock_size->setChecked(true);
    dlg.checkBox_scalemax->setChecked(true);
    dlg.exec();

    if (dlg.result() == QDialog::Accepted) {
        clearPath();

        double n_min_x,n_min_y,n_min_z;
        double n_dim_x,n_dim_y,n_dim_z;
        double scale_x,scale_y,scale_z;
        if (dlg.radioButton_stock_size->isChecked()) {
            n_min_x = s_min_x;
            n_min_y = s_min_y;
            n_min_z = s_min_z;
            n_dim_x = s_dim_x;
            n_dim_y = s_dim_y;
            n_dim_z = s_dim_z;
            if (dlg.checkBox_scalemax->isChecked()) {
                double scale = n_dim_x/m_dim_x;
                scale = std::min(n_dim_x/m_dim_x, scale);
                scale = std::min(n_dim_y/m_dim_y, scale);
                scale = std::min(n_dim_z/m_dim_z, scale);
                scale_x = scale_y = scale_z = scale;
            } else {
                if (dlg.checkBox_scalex->isChecked()) {
                    scale_x = n_dim_x/m_dim_x;
                } else {
                    scale_x = 1.0;
                }
                if (dlg.checkBox_scaley->isChecked()) {
                    scale_y = n_dim_y/m_dim_y;
                } else {
                    scale_y = 1.0;
                }
                if (dlg.checkBox_scalez->isChecked()) {
                    scale_z = n_dim_z/m_dim_z;
                } else {
                    scale_z = 1.0;
                }
            }
        } else if (dlg.radioButton_custom_size->isChecked()) {
            n_min_x = 0;
            n_min_y = 0;
            n_min_z = 0;
            n_dim_x = dlg.lineEdit_dimx->text().toDouble();
            n_dim_y = dlg.lineEdit_dimy->text().toDouble();
            n_dim_z = dlg.lineEdit_dimz->text().toDouble();
            scale_x = n_dim_x/m_dim_x;
            scale_y = n_dim_y/m_dim_y;
            scale_z = n_dim_z/m_dim_z;
        } else if (dlg.radioButton_inch_to_mm->isChecked()) {
            n_min_x = m_min_x;
            n_min_y = m_min_y;
            n_min_z = m_min_z;
            scale_x = scale_y = scale_z = 25.4;
        } else if (dlg.radioButton_mm_to_inch->isChecked()) {
            n_min_x = m_min_x;
            n_min_y = m_min_y;
            n_min_z = m_min_z;
            scale_x = scale_y = scale_z = 1/25.4;
        }
        model->resize(n_min_x * scale_x, scale_x, n_min_y * scale_y, scale_y, n_min_z * scale_z, scale_z);
        p_mminx=n_min_x * scale_x;
        p_mminy=n_min_y * scale_y;
        p_mminz=n_min_z * scale_z;
    }
}

void MainWindow::on_actionAlign_Model_to_Stock_triggered()
{
    AlignModelDialog dlg(this);

    if (model == NULL) return;
    if (stock == NULL) return;

    double m_min_x,m_min_y,m_min_z;
    double m_max_x,m_max_y,m_max_z;

    m_min_x = model->min_x();
    m_min_y = model->min_y();
    m_min_z = model->min_z();
    m_max_x = model->max_x();
    m_max_y = model->max_y();
    m_max_z = model->max_z();

    double m_dim_x = m_max_x - m_min_x;
    double m_dim_y = m_max_y - m_min_y;
    double m_dim_z = m_max_z - m_min_z;

    double s_dim_x,s_dim_y,s_dim_z;
    double s_min_x,s_min_y,s_min_z;
    double s_max_x,s_max_y,s_max_z;

    s_min_x = stock->min_x();
    s_min_y = stock->min_y();
    s_min_z = stock->min_z();
    s_dim_x = stock->dim_x();
    s_dim_y = stock->dim_y();
    s_dim_z = stock->dim_z();
    s_max_x = s_min_x + stock->dim_x();
    s_max_y = s_min_y + stock->dim_y();
    s_max_z = s_min_z + stock->dim_z();

    dlg.radioButton_xcen->setChecked(true);
    dlg.radioButton_ycen->setChecked(true);
    dlg.radioButton_zcen->setChecked(true);
    dlg.exec();

    if (dlg.result() == QDialog::Accepted) {
        clearPath();

        double min_x,min_y,min_z;
        if (dlg.radioButton_xmin->isChecked()) {
            min_x = s_min_x;
        }
        if (dlg.radioButton_xcen->isChecked()) {
            min_x = s_min_x + (s_dim_x - m_dim_x)/2;
        }
        if (dlg.radioButton_xmax->isChecked()) {
            min_x = s_min_x + s_dim_x - m_dim_x;
        }
        if (dlg.radioButton_cx->isChecked()) {
            min_x = -m_dim_x/2;
        }

        if (dlg.radioButton_ymin->isChecked()) {
            min_y = s_min_y;
        }
        if (dlg.radioButton_ycen->isChecked()) {
            min_y = s_min_y + (s_dim_y - m_dim_y)/2;
        }
        if (dlg.radioButton_ymax->isChecked()) {
            min_y = s_min_y + s_dim_y - m_dim_y;
        }
        if (dlg.radioButton_cy->isChecked()) {
            min_y = -m_dim_y/2;
        }

        if (dlg.radioButton_zmin->isChecked()) {
            min_z = s_min_z;
        }
        if (dlg.radioButton_zcen->isChecked()) {
            min_z = s_min_z + (s_dim_z - m_dim_z)/2;
        }
        if (dlg.radioButton_zmax->isChecked()) {
            min_z = s_min_z + s_dim_z - m_dim_z;
        }
        if (dlg.radioButton_cz->isChecked()) {
            min_z = -m_dim_z/2;
        }
        model->resize(min_x, 1.0, min_y, 1.0, min_z, 1.0);
        p_mminx=min_x;
        p_mminy=min_y;
        p_mminz=min_z;
    }
}


void MainWindow::on_actionRotate_Model_triggered()
{
    RotateModelDialog dlg(this);

    if (model == NULL) return;

    dlg.radioButton_xcen->setChecked(true);
    dlg.radioButton_ycen->setChecked(true);
    dlg.radioButton_zcen->setChecked(true);
    dlg.exec();

    if (dlg.result() == QDialog::Accepted) {
        clearPath();
        double rot_x=0,rot_y=0,rot_z=0;
        if (dlg.radioButton_xmin->isChecked()) {
            rot_x = -90;
        }
        if (dlg.radioButton_xcen->isChecked()) {
            rot_x = 0;
        }
        if (dlg.radioButton_xplus->isChecked()) {
            rot_x = +90;
        }

        if (dlg.radioButton_ymin->isChecked()) {
            rot_y = -90;
        }
        if (dlg.radioButton_ycen->isChecked()) {
            rot_y = 0;
        }
        if (dlg.radioButton_yplus->isChecked()) {
            rot_y = +90;
        }

        if (dlg.radioButton_zmin->isChecked()) {
            rot_z = -90;
        }
        if (dlg.radioButton_zcen->isChecked()) {
            rot_z = 0;
        }
        if (dlg.radioButton_zplus->isChecked()) {
            rot_z = +90;
        }
        model->rotate(rot_x, rot_y, rot_z);
        p_mminx=model->min_x();
        p_mminy=model->min_y();
        p_mminz=model->min_z();
    }
}



void MainWindow::on_actionExport_Model_triggered()
{
    static QString lastFile = QString::null;
    QString selectedFilter;
    QString filename = QFileDialog::getSaveFileName(this, "Export STL File", lastFile, "STL files (*.stl);;Binary STL files (*.stl)", &selectedFilter);
    std::cout << "Selected Filter: " << qPrintable(selectedFilter) << std::endl;
    if (!filename.isEmpty()) {
        lastFile = filename;
        STLExporter::ExportModel(model, filename.toStdString(), selectedFilter.contains("Binary"));
        p_modelfilename=filename;
    }
}

void MainWindow::on_actionRun_triggered()
{
    QString ss;
    if (model == NULL) return;

    double x0 = -6.5;
    double x1 = 6.5;
    double y0 = -5;
    double y1 = +5;
    size_t nx = 100;
    size_t ny = 20;
    double z0 = -1.0;
    double z1 = 4.0;
    size_t nz = 10;
    Point dir = Point(1,0,0);

    if (stock == NULL){
        stock=new Stock();
    }else{
        x0 = stock->min_x();
        x1 = stock->max_x();
        y0 = stock->min_y();
        y1 = stock->max_y();
        z0 = stock->min_z();
        z1 = stock->max_z();
    }

    SimpleCutterDialog dlg;
    dlg.m_xmin=stock->min_x();
    dlg.m_ymin=stock->min_y();
    dlg.m_zmin=stock->min_z();
    dlg.m_xmax=stock->max_x();
    dlg.m_ymax=stock->max_y();
    dlg.m_zmax=stock->max_z();
    dlg.lineEdit_lines->setPlaceholderText(QString::number(nx));
    dlg.lineEdit_points->setPlaceholderText(QString::number(ny));
    dlg.lineEdit_layers->setPlaceholderText(QString::number(nz));
    dlg.lineEdit_lines->setText(QString::number(p_runLines));
    dlg.lineEdit_points->setText(QString::number(p_runPoints));
    dlg.lineEdit_layers->setText(QString::number(p_runLayers));
    if(p_runDirectionx){
        dlg.radioButton_xdir->setChecked(p_runDirectionx);
    }else{
        dlg.radioButton_ydir->setChecked(true);
    }
    dlg.lineEditDegrees->setText(QString::number(p_runRotStep));
    dlg.checkBoxRotate->setChecked(p_runRotate);
    ss="Stock:";
    dlg.textEditStatus->append(ss);

    ss=QString("x (%1 : %2) %3").arg(stock->min_x()).arg(stock->max_x()).arg(stock->max_x()-stock->min_x());
    dlg.textEditStatus->append(ss);

    ss=QString("y (%1 : %2) %3").arg(stock->min_y()).arg(stock->max_y()).arg(stock->max_y()-stock->min_y());
    dlg.textEditStatus->append(ss);

    ss=QString("z (%1 : %2) %3").arg(stock->min_z()).arg(stock->max_z()).arg(stock->max_z()-stock->min_z());
    dlg.textEditStatus->append(ss);

    dlg.exec();


    if (dlg.result() == QDialog::Accepted) {

        clearPath();
 //       model = STLImporter::ImportModel(p_modelfilename.toStdString());

        if (!dlg.lineEdit_stepdown->text().isEmpty()) {
            double stepdown = dlg.lineEdit_stepdown->text().toDouble();
            nz = 1;
            if (stepdown > 0) {
                nz += (z1-z0)/stepdown;
            }
        } else if (!dlg.lineEdit_layers->text().isEmpty()) {
            nz = dlg.lineEdit_layers->text().toInt();
        }
        if (dlg.radioButton_xdir->isChecked()) {
            dir = Point(1,0,0);
            if (!dlg.lineEdit_lines->text().isEmpty()) {
                ny = dlg.lineEdit_lines->text().toInt();
            } else if (!dlg.lineEdit_overlap->text().isEmpty()) {
                double overlap = dlg.lineEdit_overlap->text().toDouble();
                if (overlap > 1) overlap /= 100;
                ny = (y1-y0)/(cutter->radius()*(1-overlap)*2);
            }
            if (!dlg.lineEdit_points->text().isEmpty()) {
                nx = dlg.lineEdit_points->text().toInt();
            } else if (!dlg.lineEdit_resolution->text().isEmpty()) {
                double resolution = dlg.lineEdit_resolution->text().toDouble();
                if (resolution>0) {
                    nx = (x1-x0)/resolution;
                }
            }
        }
        if (dlg.radioButton_ydir->isChecked()) {
            dir = Point(0,1,0);
            if (!dlg.lineEdit_lines->text().isEmpty()) {
                nx = dlg.lineEdit_lines->text().toInt();
            } else if (!dlg.lineEdit_overlap->text().isEmpty()) {
                double overlap = dlg.lineEdit_overlap->text().toDouble();
                if (overlap > 1) overlap /= 100;
                nx = (x1-x0)/(cutter->radius()*(1-overlap)*2);
            }
            if (!dlg.lineEdit_points->text().isEmpty()) {
                ny = dlg.lineEdit_points->text().toInt();
            } else if (!dlg.lineEdit_resolution->text().isEmpty()) {
                double resolution = dlg.lineEdit_resolution->text().toDouble();
                if (resolution>0) {
                    ny = (y1-y0)/resolution;
                }
            }
        }

        std::vector<double> zlevels;
        if (nz == 1) {
            zlevels.push_back(z0);
        } else {
            for(size_t i=0; i<nz; i++) {
                zlevels.push_back(z1 - (z1-z0)*(nz-1-i)/(nz-1));
            }
        }

//        if (!heightfield || (heightfield->width() != nx || heightfield->height() != ny))

        double ai=375.0;
        double ang=0.0;
        if(dlg.checkBoxRotate->isChecked()){
             ai = dlg.lineEditDegrees->text().toDouble();
        }

        for(ang=0.0; ang<360.0; ang+=ai){
            if (heightfield) {
                delete heightfield;
            }
            heightfield = new HeightField(x0, x1, y0, y1, z0, z1, nx, ny);
            //QProgressBar progress(this);
            //progress.setMaximum(100);
            //progress.show();
            DropCutter dropcutter;
            double workdone = 0;
            dropcutter.setAsync(true);
            dropcutter.GeneratePath(*cutter, *model, *heightfield);
            while (!dropcutter.finished(&workdone)) {
                update();
                //progress.setValue(workdone);
                QCoreApplication::processEvents();
            }
            //progress.hide();
            SimpleCutter simplecutter;
            simplecutter.m_zigzag = dlg.checkBox_zigzag->isChecked();
            simplecutter.GenerateCutPath(*heightfield, Point(stock->min_x(), stock->min_y(), stock->max_z()), dir, zlevels, paths, ang);
            if(dlg.checkBoxRotate->isChecked())model->rotate(ai,0,0);
            theGLWidget->updateGL();
        }//for ang

        /* store run vars */
        if(dlg.radioButton_xdir->isChecked()){
            p_runLines=ny;
            p_runPoints=nx;
            p_runLayers=nz;
            p_runStepover=(p_smaxy-p_sminy)/ny;
            p_runResolution=(p_smaxx-p_sminx)/nx;
            p_runStepdown=(p_smaxz-p_sminz)/nz;
        }else{
            p_runLines=nx;
            p_runPoints=ny;
            p_runLayers=nz;
            p_runStepover=(p_smaxx-p_sminx)/nx;
            p_runResolution=(p_smaxy-p_sminy)/ny;
            p_runStepdown=(p_smaxz-p_sminz)/nz;
        }
        p_runRotate=dlg.checkBoxRotate->isChecked();
        p_runRotStep=dlg.lineEditDegrees->text().toDouble();
        p_runDirectionx=dlg.radioButton_xdir->isChecked();

    }//if (dlg.result() == QDialog::Accepted)
}

void MainWindow::on_actionExport_GCode_triggered()
{
    if (paths.size()==0) return;

    static QString lastFile = QString::null;
    QString filename = QFileDialog::getSaveFileName(this, "Select GCode file", lastFile.isNull()?"data/TestModel.ngc":lastFile, "GCode files (*.ngc)");
    if (!filename.isEmpty()) {
        lastFile = filename;
        GCodeExporter exp;
        exp.ExportPath(paths, filename.toStdString());
    }
}

void MainWindow::on_actionResetView_triggered()
{
   theGLWidget->resetView();
}

void MainWindow::on_actionFrontView_triggered()
{
    theGLWidget->setTopView();
}

void MainWindow::on_actionShowCutter_triggered()
{

    theGLWidget->setShowCutter(actionShowCutter->isChecked());
    theGLWidget->updateGL();
}

void MainWindow::on_actionShowStock_triggered()
{
    theGLWidget->setShowStock(actionShowStock->isChecked());
    theGLWidget->updateGL();
}

void MainWindow::on_actionShowModel_triggered()
{
    theGLWidget->setShowModel(actionShowModel->isChecked());
    theGLWidget->updateGL();
}

void MainWindow::on_actionShowModelBoundingBox_triggered()
{
    theGLWidget->setShowModelBoundingBox(actionShowModelBoundingBox->isChecked());
    theGLWidget->updateGL();
}

void MainWindow::on_actionShowNormals_triggered()
{
    theGLWidget->setShowNormals(actionShowNormals->isChecked());
    theGLWidget->updateGL();
}

void MainWindow::on_actionShowHeightField_triggered()
{
    theGLWidget->setShowHeightField(actionShowHeightField->isChecked());
    theGLWidget->updateGL();
}

void MainWindow::on_actionShowPath_triggered()
{
    theGLWidget->setShowPath(actionShowPath->isChecked());
    theGLWidget->updateGL();
}

void MainWindow::on_actionheight_field_triggered()
{
    clearPath();
    if (model == NULL) return;
    if (heightfield) delete heightfield;

    double x0 = -6.5;
    double x1 = 6.5;
    double y0 = -5;
    double y1 = +5;
    double z0 = -1.0;
    double z1 = 4.0;
    int nx = 10;
    int ny = 5;
    Point dir = Point(0,1,0);

    if (stock) {
        x0 = stock->min_x();
        x1 = stock->max_x();
        y0 = stock->min_y();
        y1 = stock->max_y();
        z0 = stock->min_z();
        z1 = stock->max_z();
    }

    heightfield = new HeightField(x0, x1, y0, y1, z0, z1, nx, ny);

    DropCutter dropcutter;
    dropcutter.GeneratePath(*cutter, *model, *heightfield);
}

void MainWindow::on_actionZoom_in_triggered()
{
    theGLWidget->scale=theGLWidget->scale*1.1;
    theGLWidget->updateGL();
}

void MainWindow::on_actionZoom_Out_triggered()
{
    theGLWidget->scale=theGLWidget->scale/1.1;
    theGLWidget->updateGL();
}

void MainWindow::on_actiondrop_triggered()
{
    clearPath();
    if (model == NULL) return;

    Path* path = new Path();

    if (model->m_triangles.size()==1) {
        Triangle& triangle = model->m_triangles[0];

        double x0 = -1.5;
        double x1 = 5.5;
        double y0 = -1.5;
        double y1 = 5.5;
        double z0 = 0;
        double z1 = 2;
        int nx = 100;
        int ny = 100;
        if (stock) {
            x0 = stock->min_x();
            x1 = stock->max_x();
            y0 = stock->min_y();
            y1 = stock->max_y();
            z0 = stock->min_z();
            z1 = stock->max_z();
        }
        path->m_runs.resize(ny);
        for(int i=0; i<ny; i++) {
            path->m_runs[i].m_points.resize(nx);
            double y = y0 + double(i)*(y1-y0)/(ny-1);
            for(int j=0; j<nx; j++) {
                double x = x0 + double(j)*(x1-x0)/(nx-1);
                Point cl = Point(x,y,z1);
                if (cutter->drop(cl, triangle, cl)) {
                    path->m_runs[i].m_points[j] = cl;
                } else {
                    path->m_runs[i].m_points[j] = Point(x, y, z0);
                }
            }
        }
    } else {
        double x0 = -6.5;
        double x1 = 6.5;
        double y0 = -5.5;
        double y1 = 5.5;
        double z0 = 0;
        double z1 = 3;
        int nx = 100;
        int ny = 100;
        if (stock) {
            x0 = stock->min_x();
            x1 = stock->max_x();
            y0 = stock->min_y();
            y1 = stock->max_y();
            z0 = stock->min_z();
            z1 = stock->max_z();
        }
        path->m_runs.resize(ny);
        for(int i=0; i<ny; i++) {
            path->m_runs[i].m_points.resize(nx);
            double y = y0 + double(i)*(y1-y0)/(ny-1);
            for(int j=0; j<nx; j++) {
                double x = x0 + double(j)*(x1-x0)/(nx-1);
                Point cl = Point(x,y,z1);
                if (cutter->drop(cl, *model, cl)) {
                    path->m_runs[i].m_points[j] = cl;
                } else {
                    path->m_runs[i].m_points[j] = Point(x, y, z0);
                }
            }
        }
    }
    paths.push_back(path);

}

void MainWindow::on_actionpush_triggered()
{
    clearPath();
    if (model == NULL) return;

    Path* path = new Path();

    if (model->m_triangles.size() == 1) {
        Triangle& triangle = model->m_triangles[0];

        double x0 = -2.5;
        double x1 = 6.5;
        double y0 = -5;
        double y1 = 5.0;
        double z0 = -1.0;
        double z1 = 5.0;
        int nx = 100;
        int nz = 50;
        Point dir = Point(0,1,0);

        if (stock) {
            x0 = stock->min_x();
            x1 = stock->max_x();
            y0 = stock->min_y();
            y1 = stock->max_y();
            z0 = stock->min_z();
            z1 = stock->max_z();
        }
        path->m_runs.resize(nz);
        for(int i=0; i<nz; i++) {
            path->m_runs[i].m_points.resize(nx);
            double z = z0 + double(i)*(z1-z0)/(nz-1);
            for(int j=0; j<nx; j++) {
                double x = x0 + double(j)*(x1-x0)/(nx-1);
                Point cl = Point(x,y0,z);
                if (cutter->push(cl, dir, triangle, cl)) {
                    path->m_runs[i].m_points[j] = cl;
                } else {
                    path->m_runs[i].m_points[j] = Point(x, y1, z);
                }
            }
        }
    }
    else
    {
        double x0 = -6.5;
        double x1 = 6.5;
        double y0 = -5;
        double y1 = +5;
        double z0 = -1.0;
        double z1 = 4.0;
        int nx = 100;
        int nz = 50;
        Point dir = Point(0,1,0);

        if (stock) {
            x0 = stock->min_x();
            x1 = stock->max_x();
            y0 = stock->min_y();
            y1 = stock->max_y();
            z0 = stock->min_z();
            z1 = stock->max_z();
        }

        path->m_runs.resize(nz);
        for(int i=0; i<nz; i++) {
            path->m_runs[i].m_points.resize(nx);
            double z = z0 + double(i)*(z1-z0)/(nz-1);
            for(int j=0; j<nx; j++) {
                double x = x0 + double(j)*(x1-x0)/(nx-1);
                Point cl = Point(x,y0,z);
                if (cutter->push(cl, dir, *model, cl)) {
                    path->m_runs[i].m_points[j] = cl;
                } else {
                    path->m_runs[i].m_points[j] = Point(x, y1, z);
                }
            }
        }
    }
    paths.push_back(path);
}

void MainWindow::on_actionRectCut_triggered()
{
    RectCutDialog rcdlg(this);

    if(rcdlg.exec() == QDialog::Accepted)
    {
        Path* path = new Path();
        QMessageBox qmsg;
        QString ss;
    //    ss.setNum(path->m_runs.size());
    //    qmsg.setText(ss);
    //    qmsg.exec();
        path->m_runs.resize(1);
        double dimx=0.0;
        double dimy=0.0;
        double dimz=0.0;
        double dz = rcdlg.dStepDown();
        dimx=cutter->radius();
        dimx=sqrt((cutter->radius()*cutter->radius())/2.0);
        Point p = Point(rcdlg.dPosX(),rcdlg.dPosY(),rcdlg.dPosZ());
        p.m_x-=cutter->radius();
        bool bdone=false;
        while(bdone==false){
            path->m_runs[0].m_points.push_back(p);

            p.m_y+=rcdlg.dSzY();
            path->m_runs[0].m_points.push_back(p);

            p.m_x+=cutter->radius();
            p.m_y+=cutter->radius();
            p.m_rad=cutter->radius();
            path->m_runs[0].m_points.push_back(p);
            p.m_rad=-1.0;

            p.m_x+=rcdlg.dSzX();
            path->m_runs[0].m_points.push_back(p);

            p.m_x+=cutter->radius();
            p.m_y-=cutter->radius();
            p.m_rad=cutter->radius();
            path->m_runs[0].m_points.push_back(p);
            p.m_rad=-1.0;

            p.m_y-=rcdlg.dSzY();
            path->m_runs[0].m_points.push_back(p);

            p.m_x-=cutter->radius();
            p.m_y-=cutter->radius();
            p.m_rad=cutter->radius();
            path->m_runs[0].m_points.push_back(p);
            p.m_rad=-1.0;

            p.m_x-=rcdlg.dSzX();
            path->m_runs[0].m_points.push_back(p);

            p.m_x-=cutter->radius();
            p.m_y+=cutter->radius();
            p.m_rad=cutter->radius();
            path->m_runs[0].m_points.push_back(p);
            p.m_rad=-1.0;

            p.m_z-=rcdlg.dStepDown();
            if(p.m_z<(rcdlg.dPosZ()-rcdlg.dSzZ()))
            {
                p.m_z=rcdlg.dPosZ()-rcdlg.dSzZ();
                bdone=true;
            }
        }

        paths.push_back(path);
    }


}
