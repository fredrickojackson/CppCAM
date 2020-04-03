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
#include <QTimer>

#include "MainWindow.h"
#include "AboutDialog.h"
#include "PreferencesDialog.h"
#include "StockModelDialog.h"
#include "ResizeModelDialog.h"
#include "AlignModelDialog.h"
#include "RotateModelDialog.h"
#include "SimpleCutterDialog.h"

#include "dlgtoolsize.h"
#include "ui_dlgtoolsize.h"

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
#include "rackcutdlg.h"
#include "dlgholecut.h"


extern Stock* stock;
extern Model* model;
extern Cutter* cutter;
extern std::vector<Path*> paths;
extern HeightField* heightfield;

MainWindow::MainWindow() 
{
    setupUi(this);
    theGLWidget = new GLWidget(frmogl);
    m_PreferencesDialog = new PreferencesDialog();

    QObject::connect(actionTopView, SIGNAL(triggered()), theGLWidget, SLOT(setTopView()));
    QObject::connect(actionLeftView, SIGNAL(triggered()), theGLWidget, SLOT(setLeftView()));
    QObject::connect(actionFrontView, SIGNAL(triggered()), theGLWidget, SLOT(setFrontView()));
    QObject::connect(actionRightView, SIGNAL(triggered()), theGLWidget, SLOT(setRightView()));

    cutter = Cutter::CreateSphericalCutter(1.0, Point(0,0,0));
    p_runStepover=cutter->radius();
    cutter->m_cutterType="Sph";
    stock = new Stock();
    stock->m_min_x=0.0;
    stock->m_min_y=-5.0;
    stock->m_min_z=9;

    stock->m_max_x=12.0;
    stock->m_max_y=5.0;
    stock->m_max_z=15.0;

     p_runLines=100;
     p_runStepover=0.333;
     p_runPoints=125;
     p_runResolution=0.375;
     p_runLayers=5;
     p_runStepdown=2.0;
     p_runRotate=false;
     p_runRotateStep=15.0;
     p_runRotateDegrees=360.0;
     p_runDirection=0;
     p_useLine=1;
     p_smooth=0;
//     menuTest->setVisible(false);
     bEnableArcs=false;
     p_FeedSpeed=5000.0;
     p_PlungeSpeed=500.0;
     p_safez=15.0;
     qtimer = new QTimer(this);
     connect(qtimer, SIGNAL(timeout()), this, SLOT(mytimeout()));
     qtimer->setInterval(100);
     qtimer->start();
     p_modelfilename="/home/fred/projects/cppcam/data/gear_m2_12t.stl";
     model = STLImporter::ImportModel(p_modelfilename.toStdString());
     model->rotate(0,90,0);
     p_comp=0.0;



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
    QString filename = QFileDialog::getOpenFileName(this, "Select Project File", "/home/fred", "Cam files (*.cam)");
    QSettings setproj(filename,QSettings::IniFormat);
    p_modelfilename=setproj.value("p_modelfilename","~/tmp.cam").toString();

    model = STLImporter::ImportModel(p_modelfilename.toStdString());
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
}

void MainWindow::on_actionSave_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this, "Select Project File", "/home/fred", "Cam files (*.cam)");
    QSettings setproj(filename,QSettings::IniFormat);

    setproj.setValue("p_modelfilename",p_modelfilename);
    setproj.setValue("p_mminx",model->min_x());
    setproj.setValue("p_mminy",model->min_y());
    setproj.setValue("p_mminz",model->min_z());
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


    static QString lastFile = QString::null;
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
                double ds=0.0;
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
    dlg.radioButton_stock_size->setChecked(true);
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
    }
}

void MainWindow::on_actionAlign_Model_to_Stock_triggered()
{
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
    QTime mytim;
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

    SimpleCutterDialog dlg;

    dlg.m_smooth=p_smooth;
    dlg.m_xmin=stock->min_x();
    dlg.m_ymin=stock->min_y();
    dlg.m_zmin=stock->min_z();
    dlg.m_xmax=stock->max_x();
    dlg.m_ymax=stock->max_y();
    dlg.m_zmax=stock->max_z();
    ss.setNum(p_safez);
    dlg.le_SafeZ->setText(ss);
    ss.setNum(p_FeedSpeed);
    dlg.leFeedSpeed->setText(ss);
    ss.setNum(p_PlungeSpeed);
    dlg.lePlungeSpeed->setText(ss);

    //ss.setNum(1);
    dlg.leSmooth->setText("0");
    dlg.leUseLine->setText(QString::number(p_useLine));
    if(p_comp>0.0)
    {
        dlg.cbLeaveMargin->setChecked(true);
    }else
    {
        dlg.cbLeaveMargin->setChecked(false);
    }
    ss.setNum(p_comp);
    dlg.leMargin->setText(ss);


    dlg.lineEdit_lines->setText(QString::number(p_runLines));
    dlg.lineEdit_points->setText(QString::number(p_runPoints));
    dlg.lineEdit_layers->setText(QString::number(p_runLayers));
    dlg.le_SafeZ->setText(QString::number(p_safez));
    if(p_runDirection==1){
        dlg.radioButton_xdir->setChecked(true);
    }else if(p_runDirection==2){
        dlg.radioButton_ydir->setChecked(true);
    }else if(p_runDirection==3){
        dlg.rbrect->setChecked(true);
    }

    dlg.cbRotate->setChecked(p_runRotate);
    ss.setNum(p_runRotateDegrees);
    dlg.leRotateDegrees->setText(ss);
    ss.setNum(p_runRotateStep);
    dlg.leRotateStep->setText(ss);

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
        if(dlg.radioButton_xdir->isChecked()){
            p_runDirection=1;
        }else if(dlg.radioButton_ydir->isChecked()){
            p_runDirection=2;
        }else if(dlg.rbrect->isChecked()){
            p_runDirection=3;
        }
        p_runLayers=dlg.lineEdit_layers->text().toInt();
        p_smooth=dlg.leSmooth->text().toInt();
        p_useLine=dlg.leUseLine->text().toUInt();
        p_FeedSpeed=dlg.leFeedSpeed->text().toDouble();
        p_PlungeSpeed=dlg.lePlungeSpeed->text().toDouble();
        if(dlg.cbLeaveMargin->isChecked())
        {
            p_comp=dlg.leMargin->text().toDouble();
        }else
        {
            p_comp=0.0;
        }
        p_safez = dlg.le_SafeZ->text().toDouble();
        p_runRotate=dlg.cbRotate->isChecked();
        p_runRotateDegrees=dlg.leRotateDegrees->text().toDouble();
        p_runRotateStep=dlg.leRotateStep->text().toDouble();
        clearPath();
        nz = dlg.lineEdit_layers->text().toInt();

        if (dlg.radioButton_xdir->isChecked()) {
            dir = Point(1,0,0);
            if (!dlg.lineEdit_lines->text().isEmpty()) {
                ny = dlg.lineEdit_lines->text().toInt();
            } else if (!dlg.le_stepover->text().isEmpty()) {
                double overlap = dlg.le_stepover->text().toDouble();
                if (overlap > 1) overlap /= 100;
                ny = (y1-y0)/(cutter->radius()*(1-overlap)*2);
            }
            if (!dlg.lineEdit_points->text().isEmpty()) {
                nx = dlg.lineEdit_points->text().toInt();
            } else if (!dlg.le_resolution->text().isEmpty()) {
                double resolution = dlg.le_resolution->text().toDouble();
                if (resolution>0) {
                    nx = (x1-x0)/resolution;
                }
            }
        }// if(xdir is checked

        if (dlg.radioButton_ydir->isChecked()) {
            dir = Point(0,1,0);
            if (!dlg.lineEdit_lines->text().isEmpty()) {
                nx = dlg.lineEdit_lines->text().toInt();
            } else if (!dlg.le_stepover->text().isEmpty()) {
                double overlap = dlg.le_stepover->text().toDouble();
                if (overlap > 1) overlap /= 100;
                nx = (x1-x0)/(cutter->radius()*(1-overlap)*2);
            }
            if (!dlg.lineEdit_points->text().isEmpty()) {
                ny = dlg.lineEdit_points->text().toInt();
            } else if (!dlg.le_resolution->text().isEmpty()) {
                double resolution = dlg.le_resolution->text().toDouble();
                if (resolution>0) {
                    ny = (y1-y0)/resolution;
                }
            }
        }// if ydir s checked

        if (dlg.rbrect->isChecked()) {
            dir = Point(1,1,0);
            if (!dlg.lineEdit_lines->text().isEmpty()) {
                ny = dlg.lineEdit_lines->text().toInt();
            } else if (!dlg.le_stepover->text().isEmpty()) {
                double overlap = dlg.le_stepover->text().toDouble();
                if (overlap > 1) overlap /= 100;
                ny = (y1-y0)/(cutter->radius()*(1-overlap)*2);
            }
            if (!dlg.lineEdit_points->text().isEmpty()) {
                nx = dlg.lineEdit_points->text().toInt();
            } else if (!dlg.le_resolution->text().isEmpty()) {
                double resolution = dlg.le_resolution->text().toDouble();
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
        p_runLines=ny;
        p_runPoints=nx;
        p_runLayers=nz;
        p_runStepover=(stock->max_y()-stock->min_y())/ny;
        p_runResolution=(stock->max_x()-stock->min_x())/nx;
        p_runStepdown=(stock->max_z()-stock->min_z())/nz;


//        if (!heightfield || (heightfield->width() != nx || heightfield->height() != ny))

        double ai=375.0;//!!precondition
        double ang=0.0;
        double angrotatet=375.0;
        double oz = (model->m_min_z+model->m_max_z)/2.0;
        double ozc = 0.0;
        if(dlg.cbRotate->isChecked()){
             ai = dlg.leRotateStep->text().toDouble();
             angrotatet = dlg.leRotateDegrees->text().toDouble();
        }

        for(ang=0.0; ang<angrotatet; ang+=ai){
            if (heightfield) {
                delete heightfield;
            }
            heightfield = new HeightField(x0, x1, y0, y1, z0, z1, nx, ny);
            QProgressBar progress(this);
            progress.setMaximum(100);
            progress.show();
            DropCutter dropcutter;
            double workdone = 0;
            dropcutter.setAsync(false);
            dropcutter.GeneratePath(*cutter, *model, *heightfield);
            while (!dropcutter.finished(&workdone)) {
                update();
                progress.setValue(workdone);
                QCoreApplication::processEvents();
            }
            progress.hide();
            SimpleCutter simplecutter;
            simplecutter.m_radius=cutter->radius();



            if(dlg.cbLeaveMargin->isChecked())
                simplecutter.m_compmargin=dlg.leMargin->text().toDouble();
            else
                simplecutter.m_compmargin=0.0;
            simplecutter.m_stock=stock;
            simplecutter.m_useLine=p_useLine;
            simplecutter.m_smooth=dlg.leSmooth->text().toDouble();
            simplecutter.GenerateCutPath(*heightfield, Point(stock->min_x(), stock->min_y(), stock->max_z()), dir, zlevels, paths, ang);

            if(p_runRotate)
            {
                model->resize(model->m_min_x,1.0,model->m_min_y,1.0,model->m_min_z-oz,1.0);
                model->rotate(ai,0,0);
                model->resize(model->m_min_x,1.0,model->m_min_y,1.0,model->m_min_z+oz,1.0);
            }
            theGLWidget->updateGL();
        }//for ang
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

    p_rectcut=true;
    rcdlg.m_stock = stock;
    rcdlg.m_model = model;
    rcdlg.m_droundcorner=0.0;
    //rcdlg.dsbroundcorner->setValue(0.0);

    if(rcdlg.exec() == QDialog::Accepted)
    {
        bEnableArcs=true;
        clearPath();
        Path* path = new Path();
        QMessageBox qmsg;
        QString ss;


        path->m_runs.resize(1);

        Point p = Point(rcdlg.dminX(),rcdlg.dminY(),rcdlg.dmaxZ());
        p.m_x-=(cutter->radius()+rcdlg.m_droundcorner);
        path->m_runs[0].m_points.push_back(p);
        bool bdone=false;
        while(bdone==false){
            p.m_z-=rcdlg.dStepDown();
            if(p.m_z<(rcdlg.dmaxZ()-rcdlg.dSzZ()))
            {
                p.m_z=rcdlg.dmaxZ()-rcdlg.dSzZ();
                bdone=true;
            }

            p.m_rad=-1.0;
            path->m_runs[0].m_points.push_back(p);
            p.m_y+=rcdlg.dSzY();
            p.m_rad=-1.0;
            path->m_runs[0].m_points.push_back(p);

            p.m_x+=(cutter->radius()+(1.0*rcdlg.m_droundcorner));
            p.m_y+=(cutter->radius()+(1.0*rcdlg.m_droundcorner));
            p.m_rad=cutter->radius()+rcdlg.m_droundcorner;
            path->m_runs[0].m_points.push_back(p);


            p.m_rad=-1.0;
            p.m_x+=rcdlg.dSzX();
            path->m_runs[0].m_points.push_back(p);


            p.m_x+=(cutter->radius()+(1.0*rcdlg.m_droundcorner));
            p.m_y-=(cutter->radius()+(1.0*rcdlg.m_droundcorner));
            p.m_rad=cutter->radius()+rcdlg.m_droundcorner;
            path->m_runs[0].m_points.push_back(p);

            p.m_rad=-1.0;
            p.m_y-=rcdlg.dSzY();
            path->m_runs[0].m_points.push_back(p);

            p.m_x-=(cutter->radius()+(1.0*rcdlg.m_droundcorner));
            p.m_y-=(cutter->radius()+(1.0*rcdlg.m_droundcorner));
            p.m_rad=cutter->radius()+rcdlg.m_droundcorner;
            path->m_runs[0].m_points.push_back(p);

            p.m_rad=-1.0;
            p.m_x-=rcdlg.dSzX();
            path->m_runs[0].m_points.push_back(p);


            p.m_x-=(cutter->radius()+(1.0*rcdlg.m_droundcorner));
            p.m_y+=(cutter->radius()+(1.0*rcdlg.m_droundcorner));
            p.m_rad=cutter->radius()+rcdlg.m_droundcorner;
            path->m_runs[0].m_points.push_back(p);
        }

        paths.push_back(path);
    }


}

void MainWindow::on_pbTst_clicked()
{
    if(actionShowModel->isChecked())actionShowModel->setChecked(false);
    theGLWidget->setShowModel(false);
//    if(actionShowCutter->isChecked())actionShowCutter->setChecked(false);
//    theGLWidget->setShowCutter(actionShowCutter->isChecked());
    on_actionRack_Cut_triggered();
}

void MainWindow::on_MainWindow_iconSizeChanged(const QSize &iconSize)
{

}

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
        double ha = rmod;
        double hf = 1.25 * rmod;
        double hw = 2.0 * rmod;
        double c = 0.15 * rmod;
        double pf = 0.38 * rmod;

        double ttip = pf-pf*sin(prang*M_PI/180.0);
        double d0 = 0.0;
        double d1 = pf*cos(prang*M_PI/180.0);
        double d2 = (M_PI * rmod / 2.0) - d1;
        double ylin=((d2-d1)/tan(prang*M_PI/180.0))/2.0;
        //ylin=(d2-d2)*
        double hlin2 = ylin-pf*sin(prang*M_PI/180.0)+pf;
        double hd1= ylin;
        double htot = hlin2*2;
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
        double hd5= hd0;

        double dis=0.0;
        double hh=0.0;
        double hpf=0.0;
        double zl=0.0;

        double cd0=-0.475;
        double cd1=0.0;
        double cd2=0.0;
        double cd3=0.0;
        double cd4=0.0;
        double cd5=0.0;
        double ang=0.0;
//        QFile logfil("/home/fred/cppcam.log");
//        QTextStream out(&logfil);
//        logfil.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
        double xbit=0.0;
        double ybit=0.0;
        double sl = 0.0;
        double timer = 0.0;
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

xtot:
       tdist+=0.0;
        }//dis

        for(int ii=0; ii<layers; ii++)
        {
            paths.push_back(mypaths[ii]);
        }



    }//dlg accepted
}










void MainWindow::on_pbt_clicked()
{
    sim();
}

void MainWindow::on_pbt_hole_clicked()
{

    dlgHoleCut dlg;
    dlg.exec();
    while(dlg.myresult == 1)
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
    on_pbt_hole_clicked();
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
            min_z=-m_dim_z;

        }
        model->resize(min_x, 1.0, min_y, 1.0, min_z, 1.0);
    }
}
