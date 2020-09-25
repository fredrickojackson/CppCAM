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
#include <math.h>
#include <stdio.h>
#include <unistd.h>

#include <QtCore>
#include <QTime>
#include <QFileDialog>
#include <QProgressBar>
#include <QMessageBox>
#include <QStringList>
#include <QString>
#include <QTimer>
#include <QFile>

#include "Stock.h"
#include "Model.h"
#include "MainWindow.h"
#include "AboutDialog.h"
#include "PreferencesDialog.h"
#include "StockModelDialog.h"
#include "ResizeModelDialog.h"
#include "AlignModelDialog.h"
#include "RotateModelDialog.h"
#include "dlgradialcutter.h"

#include "dlgtoolsize.h"
#include "ui_dlgtoolsize.h"

#include "Path.h"
#include "DropCutter.h"
#include "SimpleCutter.h"
#include "TestModel.h"
#include "STLImporter.h"
#include "GCodeExporter.h"
#include "STLExporter.h"
#include "rectcutdialog.h"
#include "rackcutdlg.h"
#include "dlgholecut.h"


extern Stock* stock;
extern Model* model;
extern Cutter* cutter;
extern std::vector<Path*> paths;
extern HeightField* heightfield;

MainWindow::MainWindow() 
{
    //Retail_Merchandising_Signs

    setupUi(this);
    theGLWidget = new GLWidget(frmogl);
    m_PreferencesDialog = new PreferencesDialog();

    QObject::connect(actionTopView, SIGNAL(triggered()), theGLWidget, SLOT(setTopView()));
    QObject::connect(actionLeftView, SIGNAL(triggered()), theGLWidget, SLOT(setLeftView()));
    QObject::connect(actionFrontView, SIGNAL(triggered()), theGLWidget, SLOT(setFrontView()));
    QObject::connect(actionRightView, SIGNAL(triggered()), theGLWidget, SLOT(setRightView()));

    cutter = Cutter::CreateSphericalCutter(0.5, Point(0,0,0));
    p_runStepover=cutter->radius();
    cutter->m_cutterType="Sph";
    stock = new Stock();
    stock->m_min_x=10.0;
    stock->m_min_y=10.0;
    stock->m_min_z=1.0;

    stock->m_max_x=20.0;
    stock->m_max_y=20.0;
    stock->m_max_z=2.0;

     p_runLines=10;
     p_runStepover=0.333;
     p_runPoints=25;
     p_runResolution=0.375;
     p_runLayers=3;
     p_runStepdown=2.0;
     p_runRotate=false;
     p_runRotateStep=15.0;
     p_runRotateDegrees=360.0;
     p_runDirection=0;
     p_useLine=1;
     p_smooth=0;
     bEnableArcs=false;
     p_FeedSpeed=5000.0;
     p_PlungeSpeed=500.0;
     p_safez=15.0;
     qtimer = new QTimer(this);
     connect(qtimer, SIGNAL(timeout()), this, SLOT(mytimeout()));
     qtimer->setInterval(100);
     qtimer->start();
     p_modelfilename="/home/fred/projects/cppcam/data/box.stl";
     model = STLImporter::ImportModel(p_modelfilename.toStdString());
     p_comp=0.0;
     p_runs=0;



}

void MainWindow::mytimeout()
{
    QTime thetime;
    letime->setText(thetime.currentTime().toString());
}

MainWindow::~MainWindow() 
{
    delete m_PreferencesDialog;
}

void MainWindow::on_actionAbout_triggered()
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

void MainWindow::clearModelAndPath()
{
    clearPath();
    clearHeightfield();
    if (model != NULL) {
        delete model;
        model=NULL;
    }
}

void MainWindow::on_actionOpen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Select Project File", "/home/fred/projects/cppcam/data/", "Cam files (*.cam)");
    QSettings setproj(filename,QSettings::IniFormat);
    p_projectfilename=filename;
    filename=p_modelfilename;
    p_modelfilename=setproj.value("p_modelfilename","~/tmp.cam").toString();

    if(filename.compare(p_modelfilename)!=0)    model = STLImporter::ImportModel(p_modelfilename.toStdString());

    if(setproj.value("p_CutterType","Sph").toString().compare("Sph") == 0)
    {
        cutter = Cutter::CreateSphericalCutter(setproj.value("p_CutterSize",0.0).toDouble(), Point(0,0,0));
        cutter->m_cutterType="Sph";
    }else
    {
        cutter = Cutter::CreateCylindricalCutter(setproj.value("p_CutterSize",0.0).toDouble(), Point(0,0,0));
        cutter->m_cutterType="Sph";
    }


    double p_sminx=setproj.value("p_sminx",0.0).toDouble();
    stock->m_min_x=p_sminx;
    double p_sminy=setproj.value("p_sminy",0.0).toDouble();
    stock->m_min_y=p_sminy;
    double p_sminz=setproj.value("p_sminz",0.0).toDouble();
    stock->m_min_z=p_sminz;
    double p_smaxx=setproj.value("p_smaxx",0.0).toDouble();
    stock->m_max_x=p_smaxx;
    double p_smaxy=setproj.value("p_smaxy",0.0).toDouble();
    stock->m_max_y=p_smaxy;
    double p_smaxz=setproj.value("p_smaxz",0.0).toDouble();
    stock->m_max_z=p_smaxz;





    p_runLines=setproj.value("p_runLines",0.0).toInt();
    p_runPoints=setproj.value("p_runPoints",0.0).toInt();
    p_runLayers=setproj.value("p_runLayers",0.0).toInt();
    p_runStepover=setproj.value("p_runStepover",0.0).toDouble();
    p_runResolution=setproj.value("p_runResolution",0.0).toDouble();
    p_runStepdown=setproj.value("p_runStepdown",0.0).toDouble();
    p_runRotate=setproj.value("p_runRotate",0.0).toBool();
    p_runRotateDegrees=setproj.value("p_runRotateDegrees",0.0).toDouble();
    p_runRotateStep=setproj.value("p_runRotateStep",0.0).toDouble();
    p_safez=setproj.value("p_safez",0.0).toDouble();
    p_FeedSpeed=setproj.value("p_FeedSpeed",0.0).toDouble();
    p_PlungeSpeed=setproj.value("p_PlungeSpeed",0.0).toDouble();
    p_comp=setproj.value("p_comp",0.0).toDouble();
    p_runDirection=setproj.value("p_runDirection",0.0).toInt();
    p_rectcut=setproj.value("p_rectcut",0.0).toBool();
    p_useLine=setproj.value("p_useLine",0.0).toInt();
    p_smooth=setproj.value("p_smooth",0.0).toInt();
    setWindowTitle(p_projectfilename);
}

void MainWindow::on_actionSave_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this, "Select Project File", p_projectfilename, "Cam files (*.cam)");
    QSettings setproj(filename,QSettings::IniFormat);
    p_projectfilename=filename;
    setWindowTitle(filename);



    setproj.setValue("p_modelfilename",p_modelfilename);
    setproj.setValue("p_mminx",model->min_x());
    setproj.setValue("p_mminy",model->min_y());
    setproj.setValue("p_mminz",model->min_z());
    setproj.setValue("p_mmaxx",model->max_x());
    setproj.setValue("p_mmaxy",model->max_y());
    setproj.setValue("p_mmaxz",model->max_z());

    filename.setNum(cutter->radius());
    setproj.setValue("p_CutterSize",cutter->radius());
    setproj.setValue("p_CutterType",cutter->m_cutterType);
    setproj.setValue("p_sminx",stock->min_x());
    setproj.setValue("p_sminy",stock->min_y());
    setproj.setValue("p_sminz",stock->min_z());
    setproj.setValue("p_smaxx",stock->max_x());
    setproj.setValue("p_smaxy",stock->max_y());
    setproj.setValue("p_smaxz",stock->max_z());

    setproj.setValue("p_runLines",(qlonglong)p_runLines);
    setproj.setValue("p_runStepover",p_runStepover);
    setproj.setValue("p_runPoints",(qlonglong)p_runPoints);
    setproj.setValue("p_runResolution",p_runResolution);
    setproj.setValue("p_runLayers",p_runLayers);
    setproj.setValue("p_runStepdown",p_runStepdown);
    setproj.setValue("p_runRotate",p_runRotate);
    setproj.setValue("p_runRotateStep",p_runRotateStep);
    setproj.setValue("p_runRotateDegrees",p_runRotateDegrees);
    setproj.setValue("p_runDirection",p_runDirection);
    setproj.setValue("p_safez",p_safez);
    setproj.setValue("p_FeedSpeed",p_FeedSpeed);
    setproj.setValue("p_PlungeSpeed",p_PlungeSpeed);
    setproj.setValue("p_useLine",p_useLine);
    setproj.setValue("p_rectcut",p_rectcut);
    setproj.setValue("p_smooth",p_smooth);
    setproj.setValue("p_comp",p_comp);
    setproj.sync();
}

void MainWindow::on_actionImport_Model_triggered()
{


    static QString lastFile = QString("");
    QString filename = QFileDialog::getOpenFileName(this, "Import STL File", lastFile.isNull()?"data/TestModel.stl":lastFile, "STL files (*.stl)");
    if (!filename.isEmpty())
    {
        clearModelAndPath();
        lastFile = filename;
        model = STLImporter::ImportModel(filename.toStdString());
        p_modelfilename=filename;
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
            if(dlg.cbAddBorder->isChecked())
            {
                stock->m_min_x-=dlg.leMargin->text().toDouble();
                stock->m_min_y-=dlg.leMargin->text().toDouble();
                stock->m_max_x+=dlg.leMargin->text().toDouble();
                stock->m_max_y+=dlg.leMargin->text().toDouble();
            }
        }else{

            stock->m_min_x = dlg.lineEdit_minx->text().toDouble();
            stock->m_min_y = dlg.lineEdit_miny->text().toDouble();
            stock->m_min_z = dlg.lineEdit_minz->text().toDouble();
            stock->m_max_x = stock->m_min_x + dlg.lineEdit_dimx->text().toDouble();
            stock->m_max_y = stock->m_min_y + dlg.lineEdit_dimy->text().toDouble();
            stock->m_max_z = stock->m_min_z + dlg.lineEdit_dimz->text().toDouble();
        }
    }

}

void MainWindow::on_actionTool_Size_triggered()
{
    dlgToolSize dlg(this);
    dlg.m_cutterSize=cutter->radius();
    dlg.ui->lineEditSize->setText(QString::number(cutter->radius()));
    dlg.m_cuttertype=cutter->m_cutterType;
//    if(cutter->m_cutterType.compare("Sph")==0)
//        dlg.ui->radioButtonSph->setChecked(true);
//    else
//        dlg.ui->radioButtonSph->setChecked(false);

    dlg.exec();
    delete cutter;
    if(dlg.isSph()){
        cutter = Cutter::CreateSphericalCutter(dlg.m_cutterSize, Point(0,0,0));
        cutter->m_cutterType="Sph";
    }else{
        cutter = Cutter::CreateCylindricalCutter(dlg.m_cutterSize, Point(0,0,0));
        cutter->m_cutterType="Cyl";
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
    dlg.leOx->setText(QString::number(model->min_x()));
    dlg.leOy->setText(QString::number(model->min_y()));
    dlg.leOz->setText(QString::number(model->min_z()));
    dlg.radioButton_custom_size->setChecked(true);
    dlg.checkBox_scalemax->setChecked(true);
    dlg.exec();

    if (dlg.result() == QDialog::Accepted) {
        clearPath();

        double n_min_x=0.0,n_min_y=0.0,n_min_z=0.0;
        double n_dim_x=0.0,n_dim_y=0.0,n_dim_z=0.0;
        double scale_x=0.0,scale_y=0.0,scale_z=0.0;
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
            n_min_x = dlg.leOx->text().toDouble();
            n_min_y = dlg.leOy->text().toDouble();
            n_min_z = dlg.leOz->text().toDouble();
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

        model->resize(0.0, scale_x, 0.0, scale_y, 0.0, scale_z);
        model->resize(n_min_x * scale_x, 1.0, n_min_y * scale_y, 1.0, n_min_z * scale_z, 1.0);
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
    }
}

void MainWindow::sim()
{
    QString ss;
//    int hw = heightfield->width();
//    int hh = heightfield->height();
//    double sw=stock->dim_x();
//    double sh=stock->dim_y();
//    double xstep=sw/hw;
//    double ystep=sh/hh;
//    Point *plast=NULL;
    QElapsedTimer mytim;
    mytim.start();

    for(std::vector<Path*>::const_iterator p = paths.begin(); p!=paths.end(); ++p)
    {
        for(std::vector<Run>::const_iterator r = (*p)->m_runs.begin(); r!=(*p)->m_runs.end(); ++r)
        {
            for(std::vector<Point>::const_iterator pt = r->m_points.begin(); pt != r->m_points.end(); ++pt)
            {
                if(mytim.elapsed()>100)
                {
                    cutter->moveto(*pt);
                    theGLWidget->updateGL();
                    mytim.restart();
                }




































//               //Point *ppx=r->m_points[0];
//               if(isNear((*p)->rot_x,ang))
//               {
//                    double ds = cutter->radius();
//                    int ii = (pt->x()-stock->min_x())/xstep;
//                    int jj = (pt->y()-stock->min_y())/ystep;
//                    double z = pt->z();
//                    //
//                    ss.setNum(z);
//                    int dij = 1+ds/xstep;
//                    int fstart = ii-dij;
//                    int fend = ii+dij;
//                    if(fstart<0) fstart=0;
//                    if(fend>hw) fend=hw;
//                    double x1=0;
//                    double y1=0;
//                    double x2=0;
//                    double y2=0;
//                    for(int i=fstart; i<=fend;i++)
//                    {
//                        for(int j=fstart; j<=fend;j++)
//                        {
//                            z = heightfield->point(i,j);
//                            x1 = heightfield->x(i);
//                            y1 = heightfield->y(j);
//                            x2 = pt->x();
//                            y2 = pt->y();
//                            double ds1=sqrt(pow(fabs(x2-x1),2) + pow(fabs(y2-y1),2));
//                            if(ds1<ds)
//                            {
//                                if(pt->z()<z)
//                                {
//                                    //(*pt).m_z = z;
//                                }
//                            }
//                        }
//                    }

//                    //*plast = *pt;
//               }//if
            }//for

        }//for
    }//for
}


void MainWindow::on_actionExport_Model_triggered()
{
    static QString lastFile = QString();
    QString selectedFilter;
    QString filename = QFileDialog::getSaveFileName(this, "Export STL File", lastFile, "STL files (*.stl);;Binary STL files (*.stl)", &selectedFilter);
    std::cout << "Selected Filter: " << qPrintable(selectedFilter) << std::endl;
    if (!filename.isEmpty()) {
        lastFile = filename;
        STLExporter::ExportModel(model, filename.toStdString(), selectedFilter.contains("Binary"));
        p_modelfilename=filename;
    }
}


void MainWindow::on_actionExport_GCode_triggered()
{
    if (paths.size()==0) return;

    static QString lastFile = QString();
    QString filename = QFileDialog::getSaveFileName(this, "Select GCode file", lastFile.isNull()?"data/TestModel.ngc":lastFile, "GCode files (*.ngc)");
    if (!filename.isEmpty()) {
        lastFile = filename;

        GCodeExporter exp;
        exp.stk=stock;
        exp.mdl=model;
        exp.m_radial=p_runRotate;
        exp.bEnableArcs=bEnableArcs;
        exp.safez=p_safez;
        exp.feedspeed=p_FeedSpeed;
        exp.plungespeed=p_PlungeSpeed;
        exp.cutter_radius=cutter->radius();
        exp.cutter_type=cutter->m_cutterType;
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
    Point p;// = Point(rcdlg.dsbminX->value(),rcdlg.dsbminX->value(),rcdlg.dsbminX->value());//
    double f_boxw=0.0;
    double f_boxcr=0.0;
    double f_boxh=0.0;


    RectCutDialog rcdlg(this);

    p_rectcut=true;
    rcdlg.m_stock = stock;
    rcdlg.m_model = model;
    rcdlg.m_droundcorner=0.0;
    //rcdlg.dsbroundcorner->setValue(0.0);

    if(rcdlg.exec() == QDialog::Accepted)
    {

        f_boxw=rcdlg.ui->dsbsizeX->value();
        f_boxh=rcdlg.ui->dsbsizeY->value();
        if(rcdlg.ui->cbInside->isChecked())
        {
            f_boxw-=2.0*cutter->radius();
            f_boxh-=2.0*cutter->radius();
            f_boxcr=0.0;
        }



        p.m_x=rcdlg.ui->dsbminX->value();
        p.m_y=rcdlg.ui->dsbminY->value();




        bEnableArcs=true;
        clearPath();
        Path* path = new Path();
        QMessageBox qmsg;
        QString ss;

        // always start at x=minx,y=miny,z=maxz + specified bit width comp + radius of roundcorner. (inside,outside or none.)
//

        path->m_runs.resize(1);
        p.m_z=rcdlg.ui->dsbmaxZ->value();
        while(p.m_z>=rcdlg.ui->dsbminZ->value())
        {

            p.m_x=rcdlg.ui->dsbminX->value();
            p.m_y=rcdlg.ui->dsbminY->value();

            if(rcdlg.ui->cbInside->isChecked())
            {
                p.m_x+=cutter->radius();
                p.m_y+=cutter->radius();

            }

            if(p.m_z==rcdlg.ui->dsbmaxZ->value())
                p.m_rad=-1.0;
            else
                p.m_rad=f_boxcr;
            path->m_runs[0].m_points.push_back(p);
//first point



            if(!rcdlg.ui->cbClockwise->isChecked())
            {
                p.m_x+=f_boxw;
                p.m_rad=-1.0;
            }
            path->m_runs[0].m_points.push_back(p);




            if(!rcdlg.ui->cbClockwise->isChecked())
            {
                 p.m_y+=f_boxh;
                 p.m_rad=-1.0;
            }
            path->m_runs[0].m_points.push_back(p);


            if(!rcdlg.ui->cbClockwise->isChecked())
            {
                p.m_x-=f_boxw;
                p.m_rad=-1.0;
            }
            path->m_runs[0].m_points.push_back(p);


            if(!rcdlg.ui->cbClockwise->isChecked())
            {
                p.m_y-=f_boxh;
                p.m_rad=-1.0;
            }
            path->m_runs[0].m_points.push_back(p);






            p.m_z-=rcdlg.ui->dsbStepDown->value();
        }//while

        paths.push_back(path);
    }


}

//void MainWindow::on_pbTst_clicked()
//{
//    if(actionShowModel->isChecked())actionShowModel->setChecked(false);
//    theGLWidget->setShowModel(false);
////    if(actionShowCutter->isChecked())actionShowCutter->setChecked(false);
////    theGLWidget->setShowCutter(actionShowCutter->isChecked());
//    on_actionRack_Cut_triggered();
//}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    // Your code here.
    int xx = this->width();
    xx-=150;
    int yy = this->height()-50;
    //frmogl->setFrameRect(QRect(150,50,xx,yy));
    frmogl->setGeometry(150,50,xx-10,yy-10);
//    theGLWidget->resizeGL(xx,yy);
    theGLWidget->move(0,0);
    theGLWidget->resize(xx,yy);
//    theGLWidget->updateGL();
}

void MainWindow::mouseMoveEvent(QMouseEvent *ev)
{
    QString qs=QString::number(ev->x());
    qs.append(",").append(QString::number(ev->y()));
    leStat->setText(qs);
}

void MainWindow::on_actionRack_Cut_triggered()
{
    rackcutdlg dlg;
    if (dlg.exec() == QDialog::Accepted)
    {

        clearPath();

        double stepdown=dlg.ui->dsbStepDown->value();
        double rwid = dlg.ui->dsbWid->value();


        int layers=1;


        Path *path = NULL;

        if(dlg.ui->rbSideCut->isChecked())
        {
            layers=rwid/stepdown+1;
        }else
            layers=1;

        Path *mypaths[layers];
        for(int ii=0; ii<=layers; ii++)
        {
            mypaths[ii]=new Path();
            mypaths[ii]->m_runs.resize(1);
        }




        bEnableArcs=true;

        double rmod = dlg.ui->dsbMod->value();
        double rlen = dlg.ui->dsbLen->value();
        double step = dlg.ui->dsbStep->value();
        double stepover = dlg.ui->dsbStepOver->value();
        double pitch = M_PI * rmod;
        double prang = dlg.ui->dsbPrAngle->value();
        double c = 0.15 * rmod;
        double pf = 0.38 * rmod;

        double d1 = pf*cos(prang*M_PI/180.0);
        double d2 = (M_PI * rmod / 2.0) - d1;
        double ylin=((d2-d1)/tan(prang*M_PI/180.0))/2.0;
        //ylin=(d2-d2)*
        double hd1= ylin;
        double hd2 = -ylin;
        double hd0=ylin-pf*sin(prang*M_PI/180.0)+pf;



        double d3 = (M_PI * rmod / 2.0) + d1;
        double hd3=hd2;

        double d4 =3*d1+2*(d2-d1);
        //hd1 = hd0-pf*sin(prang*M_PI/180.0);
        double hd4= hd1;
        double slope1=(hd2-hd1)/(d2-d1);
        double slope2=(hd4-hd3)/(d4-d3);



        double d5 = M_PI * rmod;

        double dis=0.0;
        double hpf=0.0;
        double zl=0.0;

//        QFile logfil("/home/fred/cppcam.log");
//        QTextStream out(&logfil);
//        logfil.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
        double xbit=0.0;
        double ybit=0.0;
        double sl = 0.0;
        double yd=0.0;
        double fudge=-999.0;
        double tdist=0.0;
        double clry=0.0;
        double zlast=999.0;
        bool arcflag = 0;
        Point p = Point(-rwid/2.0,0.0,hd0);
        cutter->moveto(p);
        for(dis=0;dis<=rlen;dis+=step)
        {

            tdist=fmod(dis,pitch);
//            out << "tdist=" << tdist << "\n";
            if(tdist<d1)
            {
                double dy=pow((pow(pf,2.0)-pow(tdist,2.0)),0.5);
                hpf=ylin-pf*sin(prang*M_PI/180.0)+dy;
                zl=hpf;
                if(tdist<=step)
                    arcflag=false;
                if(tdist==0.0)
                {
                    ybit=cutter->radius();
                    xbit=0.0;
                }else
                {
                    sl=(hd0-zl)/tdist;
                    sl = (zl - (ylin-pf*sin(prang*M_PI/180.0)))/(tdist);

                    double ang = atan(sl)*180.0/M_PI;
                    ybit=cutter->radius()*sin(ang*M_PI/180.0);
                    xbit=cutter->radius()*cos(ang*M_PI/180.0);
                }
                if(zl>(hd0-c))
                {
                    zl=hd0-c;
                    xbit=0.0;
                    ybit=cutter->radius();
                }else
                {
                    if(arcflag==false)
                    {
                        arcflag=true;
                        p.m_rad=cutter->radius();
                    }
                }
            }else if(tdist>=d1 && tdist<d2)
            {
                arcflag=false;
                hpf=hd1-(tdist-d1)/tan(prang*M_PI/180.0);
                zl=hd1+slope1*(tdist-d1);
                ybit=cutter->radius()*sin(prang*M_PI/180.0);
                xbit=cutter->radius()*cos(prang*M_PI/180.0);
            }else if(tdist>=d2 && tdist<d3)
            {
                if(tdist<(M_PI*rmod/2.0))
                {
                    yd=pow(pow(pf,2.0)-pow(((pitch/2.0)-(tdist)),2.0),0.5);
                    zl=-ylin+pf*sin(prang*M_PI/180.0)-yd;
                    sl = (zl - (ylin-pf*sin(prang*M_PI/180.0)))/((pitch/2.0)-(tdist));

                    //sl=-(hd0-zl)/((pitch/2.0)-(tdist));
                    double dy = yd;
                    double dx = (M_PI*rmod/2.0)-tdist;
                    dy=sqrt(pow(pf,2.0)-pow(dx,2.0));
                    sl=dy/dx;
                    double ang = atan(sl)*180.0/M_PI;
                    ybit=cutter->radius()*sin(ang*M_PI/180.0);
                    xbit=cutter->radius()*cos(ang*M_PI/180.0);
                }else
                {
                    yd=pow(pow(pf,2.0)-pow(-((pitch/2.0)-(tdist)),2.0),0.5);
                    zl=-ylin+pf*sin(prang*M_PI/180.0)-yd;
                    double dy = yd;
                    double dx = (M_PI*rmod/2.0)-tdist;
                    dy=sqrt(pow(pf,2.0)-pow(dx,2.0));
                    sl=fabs(dy/dx);
                    double ang = atan(sl)*180.0/M_PI;
                    ybit=cutter->radius()*sin(ang*M_PI/180.0);
                    xbit=-cutter->radius()*cos(ang*M_PI/180.0);
                }
            }else if(tdist>=d3 && tdist<d4)
            {
                zl=hd3+slope2*(tdist-d3);
                xbit=-cutter->radius()*cos(prang*M_PI/180.0);
                ybit=cutter->radius()*sin(prang*M_PI/180.0);
            }else if(tdist>=d4 && tdist<d5)
            {
                double dy=pow((pow(pf,2.0)-pow(d5-tdist,2.0)),0.5);
                hpf=ylin-pf*sin(prang*M_PI/180.0)+dy;
                zl=hpf;
                sl = (zl - (ylin-pf*sin(prang*M_PI/180.0)))/(pitch-tdist);
                double ang = atan(sl)*180.0/M_PI;
                ybit=cutter->radius()*sin(ang*M_PI/180.0);
                xbit=-cutter->radius()*cos(ang*M_PI/180.0);
                if(zl>(hd0-c))
                {
                    zl=hd0-c;
                    xbit=0.0;
                    ybit=cutter->radius();
                    if(arcflag==false)
                    {
                        arcflag=true;
                        p.m_rad=cutter->radius();
                    }
                }            }
            if(fudge == -999.0)fudge=zl+ybit+cutter->radius();




            if(dlg.ui->rbTopCut->isChecked())
            {
                p.m_z=zl+ybit+cutter->radius()-fudge;
                p.m_x=rwid/2.0;
                p.m_y=dis+xbit;
                path->m_runs[0].m_points.push_back(p);
                p.m_x=-rwid/2.0;
                path->m_runs[0].m_points.push_back(p);
                if(dlg.ui->cbClear->isChecked())
                {
                    if(tdist <= pitch/2.0 && fabs(zlast-p.m_z) >= stepdown)
                    {

                        clry=(pitch/2.0-(tdist+xbit))*2.0;
                        for(double clrwid=-rwid/2.0;clrwid<=rwid/2.0;clrwid+=stepover)
                        {
                            p.m_x=clrwid;
                            p.m_y=dis+xbit;
                            path->m_runs[0].m_points.push_back(p);
                            p.m_y=dis+xbit+clry;
                            path->m_runs[0].m_points.push_back(p);
                        }
                        zlast=p.m_z;
                    }
                }
            }else
            {
                p.m_y=zl+ybit+cutter->radius()-fudge;
                p.m_x=dis+xbit;
                p.m_z=0;
                for(int ii=0; ii<layers; ii++)
                {
                    p.m_z=-ii*stepdown;
                    mypaths[ii]->m_runs[0].m_points.push_back(p);
                }
                p.m_rad=-1.0;
            }


       tdist+=0.0;
        }//dis

        for(int ii=0; ii<layers; ii++)
        {
            paths.push_back(mypaths[ii]);
        }



    }//dlg accepted
}




void MainWindow::hole_cut()
{

    dlgHoleCut dlg;
    dlg.exec();
    if(dlg.myresult == 1)
    {
        bEnableArcs=true;
        double stepdown=dlg.ui->lestepdown->text().toDouble();
        double radius = dlg.ui->leradius->text().toDouble();
        Path *path = new Path();
        path->m_runs.resize(1);
        Point p;
        if (dlg.ui->rbinside->isChecked())
        {
            p.m_x=dlg.ui->leposx->text().toDouble()-radius+cutter->radius();
        }else
        {
            p.m_x=dlg.ui->leposx->text().toDouble()-radius-cutter->radius();
        }
        p.m_y=dlg.ui->leposy->text().toDouble();

        for(p.m_z=dlg.ui->leposz->text().toDouble();p.m_z>=(dlg.ui->leposz->text().toDouble()-dlg.ui->ledepth->text().toDouble());p.m_z-=stepdown)
        {
            p.m_rad=0.0;
            path->m_runs[0].m_points.push_back(p);

            if (dlg.ui->rbinside->isChecked())
            {
                p.m_rad=radius-cutter->radius();
                p.m_x=dlg.ui->leposx->text().toDouble()+radius-cutter->radius();
            }else
            {
                p.m_rad=radius+cutter->radius();
                p.m_x=dlg.ui->leposx->text().toDouble()+radius+cutter->radius();
            }
            path->m_runs[0].m_points.push_back(p);
            if (dlg.ui->rbinside->isChecked())
            {
                p.m_x=dlg.ui->leposx->text().toDouble()-radius+cutter->radius();
            }else
            {
                p.m_x=dlg.ui->leposx->text().toDouble()-radius-cutter->radius();
            }
            path->m_runs[0].m_points.push_back(p);

        }
        paths.push_back(path);
        dlg.exec();
    }
}

void MainWindow::on_actionHole_Cut_triggered()
{
    hole_cut();
}

void MainWindow::on_actionClear_Path_triggered()
{
    clearPath();
}

void MainWindow::on_actionAlign_Model_triggered()
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
//    double s_max_x,s_max_y,s_max_z;

    s_min_x = stock->min_x();
    s_min_y = stock->min_y();
    s_min_z = stock->min_z();
    s_dim_x = stock->dim_x();
    s_dim_y = stock->dim_y();
    s_dim_z = stock->dim_z();
//    s_max_x = s_min_x + stock->dim_x();
//    s_max_y = s_min_y + stock->dim_y();
//    s_max_z = s_min_z + stock->dim_z();

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
            min_z=-m_dim_z;

        }
        model->resize(min_x, 1.0, min_y, 1.0, min_z, 1.0);
    }
}

void MainWindow::on_actionRadial_Cut_triggered()
{
    QString ss;
    HeightField* hf[72];
    p_rectcut=false;
    bEnableArcs=false;
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

    dlgRadialCutter dlg;

    dlg.m_smooth=p_smooth;
    dlg.m_xmin=stock->min_x();
    dlg.m_ymin=stock->min_y();
    dlg.m_zmin=stock->min_z();
    dlg.m_xmax=stock->max_x();
    dlg.m_ymax=stock->max_y();
    dlg.m_zmax=stock->max_z();
    ss.setNum(p_safez);
    dlg.ui->le_SafeZ->setText(ss);
    ss.setNum(p_FeedSpeed);
    dlg.ui->leFeedSpeed->setText(ss);
    ss.setNum(p_PlungeSpeed);
    dlg.ui->lePlungeSpeed->setText(ss);

    //ss.setNum(1);
    dlg.ui->leSmooth->setText("0");
    dlg.ui->leUseLine->setText(QString::number(p_useLine));
    if(p_comp>0.0)
    {
        dlg.ui->cbLeaveMargin->setChecked(true);
        dlg.ui->leMargin->setText(QString::number(p_comp));
    }else
    {
        dlg.ui->cbLeaveMargin->setChecked(false);
    }
    ss.setNum(p_comp);
    dlg.ui->leMargin->setText(ss);


    dlg.ui->lineEdit_lines->setText(QString::number(p_runLines));
    dlg.ui->lineEdit_points->setText(QString::number(p_runPoints));
    dlg.ui->lineEdit_layers->setText(QString::number(p_runLayers));
    dlg.ui->le_SafeZ->setText(QString::number(p_safez));
    if(p_runDirection==1){
        dlg.ui->radioButton_xdir->setChecked(true);
    }else if(p_runDirection==2){
        dlg.ui->radioButton_ydir->setChecked(true);
    }else if(p_runDirection==3){
        dlg.ui->rbrect->setChecked(true);
    }

    dlg.ui->cbRotate->setChecked(p_runRotate);
    ss.setNum(p_runRotateDegrees);
    dlg.ui->leRotateDegrees->setText(ss);
    ss.setNum(p_runRotateStep);
    dlg.ui->leRotateStep->setText(ss);

    ss="Stock:";
    dlg.ui->textEditStatus->append(ss);

    ss=QString("x (%1 : %2) %3").arg(stock->min_x()).arg(stock->max_x()).arg(stock->max_x()-stock->min_x());
    dlg.ui->textEditStatus->append(ss);

    ss=QString("y (%1 : %2) %3").arg(stock->min_y()).arg(stock->max_y()).arg(stock->max_y()-stock->min_y());
    dlg.ui->textEditStatus->append(ss);

    ss=QString("z (%1 : %2) %3").arg(stock->min_z()).arg(stock->max_z()).arg(stock->max_z()-stock->min_z());
    dlg.ui->textEditStatus->append(ss);


    ss="Model:";
    dlg.ui->textEditStatus->append(ss);

    ss=QString("x (%1 : %2) %3").arg(model->min_x()).arg(model->max_x()).arg(model->max_x()-model->min_x());
    dlg.ui->textEditStatus->append(ss);

    ss=QString("y (%1 : %2) %3").arg(model->min_y()).arg(model->max_y()).arg(model->max_y()-model->min_y());
    dlg.ui->textEditStatus->append(ss);

    ss=QString("z (%1 : %2) %3").arg(model->min_z()).arg(model->max_z()).arg(model->max_z()-model->min_z());
    dlg.ui->textEditStatus->append(ss);

    dlg.exec();


    if (dlg.result() == QDialog::Accepted)
    {
        if(dlg.ui->radioButton_xdir->isChecked()){
            p_runDirection=1;
        }else if(dlg.ui->radioButton_ydir->isChecked()){
            p_runDirection=2;
        }else if(dlg.ui->rbrect->isChecked()){
            p_runDirection=3;
        }
        p_runLayers=dlg.ui->lineEdit_layers->text().toInt();
        p_smooth=dlg.ui->leSmooth->text().toInt();
        p_useLine=dlg.ui->leUseLine->text().toUInt();
        p_FeedSpeed=dlg.ui->leFeedSpeed->text().toDouble();
        p_PlungeSpeed=dlg.ui->lePlungeSpeed->text().toDouble();
        if(dlg.ui->cbLeaveMargin->isChecked())
        {
            p_comp=dlg.ui->leMargin->text().toDouble();
        }else
        {
            p_comp=0.0;
        }
        p_safez = dlg.ui->le_SafeZ->text().toDouble();
        p_runRotate=dlg.ui->cbRotate->isChecked();
        if(p_runRotate==false)
            p_runRotateDegrees=0.0;
        else
            p_runRotateDegrees=dlg.ui->leRotateDegrees->text().toDouble();

        p_runRotateStep=dlg.ui->leRotateStep->text().toDouble();
        clearPath();
        nz = dlg.ui->lineEdit_layers->text().toInt();

        if (dlg.ui->radioButton_xdir->isChecked()) {
            dir = Point(1,0,0);
            if (!dlg.ui->lineEdit_lines->text().isEmpty()) {
                ny = dlg.ui->lineEdit_lines->text().toInt();
            } else if (!dlg.ui->le_stepover->text().isEmpty()) {
                double overlap = dlg.ui->le_stepover->text().toDouble();
                if (overlap > 1) overlap /= 100;
                ny = (y1-y0)/(cutter->radius()*(1-overlap)*2);
            }
            if (!dlg.ui->lineEdit_points->text().isEmpty()) {
                nx = dlg.ui->lineEdit_points->text().toInt();
            } else if (!dlg.ui->le_resolution->text().isEmpty()) {
                double resolution = dlg.ui->le_resolution->text().toDouble();
                if (resolution>0) {
                    nx = (x1-x0)/resolution;
                }
            }
        }// if(xdir is checked

        if (dlg.ui->radioButton_ydir->isChecked()) {
            dir = Point(0,1,0);
            if (!dlg.ui->lineEdit_lines->text().isEmpty()) {
                nx = dlg.ui->lineEdit_lines->text().toInt();
            } else if (!dlg.ui->le_stepover->text().isEmpty()) {
                double overlap = dlg.ui->le_stepover->text().toDouble();
                if (overlap > 1) overlap /= 100;
                nx = (x1-x0)/(cutter->radius()*(1-overlap)*2);
            }
            if (!dlg.ui->lineEdit_points->text().isEmpty()) {
                ny = dlg.ui->lineEdit_points->text().toInt();
            } else if (!dlg.ui->le_resolution->text().isEmpty()) {
                double resolution = dlg.ui->le_resolution->text().toDouble();
                if (resolution>0) {
                    ny = (y1-y0)/resolution;
                }
            }
        }// if ydir s checked

        if (dlg.ui->rbrect->isChecked()) {
            dir = Point(1,1,0);
            if (!dlg.ui->lineEdit_lines->text().isEmpty()) {
                ny = dlg.ui->lineEdit_lines->text().toInt();
            } else if (!dlg.ui->le_stepover->text().isEmpty()) {
                double overlap = dlg.ui->le_stepover->text().toDouble();
                if (overlap > 1) overlap /= 100;
                ny = (y1-y0)/(cutter->radius()*(1-overlap)*2);
            }
            if (!dlg.ui->lineEdit_points->text().isEmpty()) {
                nx = dlg.ui->lineEdit_points->text().toInt();
            } else if (!dlg.ui->le_resolution->text().isEmpty()) {
                double resolution = dlg.ui->le_resolution->text().toDouble();
                if (resolution>0) {
                    nx = (x1-x0)/resolution;
                }
            }
        }// if rectangular is checked

        std::vector<double> zlevels;
        if (nz == 1) {
            zlevels.push_back(z0);
        } else {
            for(size_t i=0; i<nz; i++) {
                zlevels.push_back(z1 - (z1-z0)*(nz-1-i)/(nz-1));
            }
        }
        p_runLines=dlg.ui->lineEdit_lines->text().toInt();
        p_runPoints=dlg.ui->lineEdit_points->text().toInt();
        p_runLayers=nz;
        p_runStepover=dlg.ui->le_stepover->text().toDouble();
        p_runResolution=dlg.ui->le_resolution->text().toDouble();
        p_runStepdown=(stock->max_z()-stock->min_z())/nz;
        p_dir=dir;

//        if (!heightfield || (heightfield->width() != nx || heightfield->height() != ny))

        double ai=375.0;   // !!precondition
        double ang=0.0;
        double angrotatet=375.0;
        double oz = (model->m_min_z+model->m_max_z)/2.0;
        if(dlg.ui->cbRotate->isChecked()){
             ai = dlg.ui->leRotateStep->text().toDouble();
             angrotatet = dlg.ui->leRotateDegrees->text().toDouble();
        }
        for(size_t i=0; i<nz; i++)
        {
            double Zlevel=0.0;
            if(nz>1)
            {
                Zlevel=z0 + (z1-z0)*(nz-1-i)/(nz-1);
            }else
            {
                Zlevel=z0;
            }

            for(ang=0.0; ang<angrotatet; ang+=ai)
            {
                int hfi=ang/ai;
                if(i==0)
                {
                    hf[hfi] = new HeightField(x0, x1, y0, y1, z0, z1, nx, ny);
                    QProgressBar progress(this);
                    progress.setMaximum(100);
                    progress.show();
                    DropCutter dropcutter;
                    double workdone = 0;
                    dropcutter.setAsync(false);
                    dropcutter.GeneratePath(*cutter, *model, *hf[hfi]);
                    while (!dropcutter.finished(&workdone)) {
                        update();
                        progress.setValue(workdone);
                        QCoreApplication::processEvents();
                    }
                    progress.hide();
                }
                SimpleCutter simplecutter;
                simplecutter.m_radius=cutter->radius();
                simplecutter.m_noretrace=true;



                if(dlg.ui->cbLeaveMargin->isChecked())
                    simplecutter.m_compmargin=dlg.ui->leMargin->text().toDouble();
                else
                    simplecutter.m_compmargin=0.0;
                simplecutter.m_stock=stock;
                simplecutter.m_useLine=p_useLine;
                simplecutter.m_smooth=dlg.ui->leSmooth->text().toDouble();
                simplecutter.m_radialcut=ai;
                simplecutter.m_radialcutdepth=oz;
                simplecutter.GenerateCutPath_radial(*hf[hfi], Point(stock->min_x(), stock->min_y(), stock->max_z()), dir, Zlevel, paths, ang);

                if(p_runRotate)
                {
                    model->resize(model->m_min_x,1.0,model->m_min_y,1.0,model->m_min_z-oz,1.0);
                    model->rotate(ai,0,0);
                    model->resize(model->m_min_x,1.0,model->m_min_y,1.0,model->m_min_z+oz,1.0);
                }
                theGLWidget->updateGL();
            }//for ang
        }//for i
    }//if (dlg.ui->result() == QDialog::Accepted)
}


void MainWindow::on_pbAdd_clicked()
{

    if(p_runDirection==1 && p_runRotate==false){ r_runs[p_runs].type=1; r_runs[p_runs].dir=Point(1,0,0);}
    if(p_runDirection==2 && p_runRotate==false){r_runs[p_runs].type=2; r_runs[p_runs].dir=Point(0,1,0);}
    if(p_runDirection==3 && p_runRotate==false){r_runs[p_runs].type=3; r_runs[p_runs].dir=Point(1,1,0);}
    if(p_runDirection==1 && p_runRotate==true){r_runs[p_runs].type=4; r_runs[p_runs].dir=Point(1,0,0);}
    if(p_runDirection==2 && p_runRotate==true){r_runs[p_runs].type=5; r_runs[p_runs].dir=Point(0,1,0);}
    if(p_runDirection==3 && p_runRotate==true){r_runs[p_runs].type=6; r_runs[p_runs].dir=Point(1,1,0);}
    r_runs[p_runs].dir=p_dir;//

    r_runs[p_runs].r_useLine=p_useLine;//
    r_runs[p_runs].smooth=p_smooth;

    r_runs[p_runs].stock=new Stock;
    r_runs[p_runs].stock->copy(stock);

    if(cutter->m_cutterType.compare("Sph"))
        r_runs[p_runs].cutter=Cutter::CreateSphericalCutter(cutter->radius(), Point(0,0,0));
    else
        r_runs[p_runs].cutter=Cutter::CreateCylindricalCutter(cutter->radius(), Point(0,0,0));

    //r_runs[p_runs].model=model;

    r_runs[p_runs].lines=p_runLines;
    r_runs[p_runs].points=p_runPoints;
    r_runs[p_runs].layers=p_runLayers;
    r_runs[p_runs].safez=p_safez;
    r_runs[p_runs].fspd=p_FeedSpeed;
    r_runs[p_runs].pspd=p_PlungeSpeed;
    r_runs[p_runs].margin=p_comp;
    if(p_runRotate)
    {
        r_runs[p_runs].rotatetotal=p_runRotateDegrees;
        r_runs[p_runs].rotatestep=p_runRotateStep;
    }else
    {
        r_runs[p_runs].rotatetotal=0.0;
        r_runs[p_runs].rotatestep=0.0;
    }
    p_runs++;
    sbRuncnt->setValue(p_runs);
}

void MainWindow::on_pbRemove_clicked()
{
    if(p_runs>=0)p_runs--;
    delete r_runs[p_runs].stock;
    delete r_runs[p_runs].cutter;
    sbRuncnt->setValue(p_runs);
}

void MainWindow::on_pbRun_clicked()
{
    unsigned int ct=0;
    bool bx;
    for(ct=0;ct<p_runs;ct++)
    {
        bx=radialcut(r_runs[ct]);
    }
}

bool MainWindow::radialcut(r_run myrun)
{

    unsigned int nz=myrun.layers;

    double x0=myrun.stock->m_min_x;
    double x1=myrun.stock->m_max_x;
    double y0=myrun.stock->m_min_y;
    double y1=myrun.stock->m_max_y;
    double z0=myrun.stock->m_min_z;
    double z1=myrun.stock->m_max_z;
    double ang=0.0;
    double angrotatet=myrun.rotatetotal;
    double ai=myrun.rotatestep;
    HeightField* hf[72];
    int nx=0;
    int ny=0;
    if(myrun.type==1 || myrun.type==3 || myrun.type==4 || myrun.type==6)
    {
        nx=myrun.points;
        ny=myrun.lines;
    }else
    {
        nx=myrun.lines;
        ny=myrun.points;
    }
    double oz = (model->m_min_z+model->m_max_z)/2.0;






    ai=1375.0;   // !!precondition
    ang=0.0;
    angrotatet=375.0;
    oz = (model->m_min_z+model->m_max_z)/2.0;
    if(myrun.rotatetotal){
         ai = myrun.rotatestep;
         angrotatet=myrun.rotatetotal;
    }


    for(size_t i=0; i<nz; i++)
    {
        double Zlevel=0.0;
        if(nz>1)
        {
            Zlevel=z0 + (z1-z0)*(nz-1-i)/(nz-1);
        }else
        {
            Zlevel=z0;
        }

        for(ang=0.0; ang<angrotatet; ang+=ai)
        {
            int hfi=ang/ai;
            if(i==0)
            {
                hf[hfi] = new HeightField(x0, x1, y0, y1, z0, z1, nx, ny);
                QProgressBar progress(this);
                progress.setMaximum(100);
                progress.show();

                DropCutter dropcutter;
                double workdone = 0;
                dropcutter.setAsync(false);
                dropcutter.GeneratePath(*myrun.cutter, *model, *hf[hfi]);
                while (!dropcutter.finished(&workdone)) {
                    update();
                    QCoreApplication::processEvents();
                }
                progress.hide();
            }
            SimpleCutter simplecutter;
            simplecutter.m_radius=myrun.cutter->radius();
            simplecutter.m_noretrace=true;



            simplecutter.m_compmargin=myrun.margin;
            simplecutter.m_stock=myrun.stock;
            simplecutter.m_useLine=myrun.r_useLine;
            simplecutter.m_smooth=myrun.smooth;
            simplecutter.m_radialcut=ai;
            simplecutter.m_radialcutdepth=oz;
            simplecutter.GenerateCutPath_radial(*hf[hfi], Point(myrun.stock->min_x(), myrun.stock->min_y(), myrun.stock->max_z()), myrun.dir, Zlevel, paths, ang);

            if(myrun.rotatetotal)
            {
                model->resize(model->m_min_x,1.0,model->m_min_y,1.0,model->m_min_z-oz,1.0);
                model->rotate(ai,0,0);
                model->resize(model->m_min_x,1.0,model->m_min_y,1.0,model->m_min_z+oz,1.0);
            }
            theGLWidget->updateGL();
        }//for ang
    }//for i
    return true;
}

void MainWindow::on_pbClr_clicked()
{
    clearPath();
}

void MainWindow::logit(QString qstr)
{
    QFile logfile("logfile.txt");
    logfile.open(QIODevice::Append |  QIODevice::Text);
    logfile.write((char *)qstr.data());
    logfile.write(" \n");
    logfile.write(" ");
}

void MainWindow::on_pbUp_clicked()
{
    sbRuncnt->setValue(sbRuncnt->value()+1);
}

void MainWindow::on_pbDown_clicked()
{
    sbRuncnt->setValue(sbRuncnt->value()-1);
}
