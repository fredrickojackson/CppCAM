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
#include <QFile>
#include <QSettings>
#include <QString>

#include "GCodeExporter.h"
#include "dlggcodeprop.h"
#include "ui_dlggcodeprop.h"

QSettings* settings = NULL;

GCodeExporter::GCodeExporter()
{
    if (settings == NULL) {
        settings = new QSettings("/home/fred/GCode.conf", QSettings::IniFormat);
    }
    safez = settings->value("default_safez").toDouble();
}


bool
GCodeExporter::ExportPath(const std::vector<Path*>& paths, std::string filename)
{
    QFile gc(QString::fromStdString(filename));
    if (!gc.open(QIODevice::WriteOnly)) {
        return false;
    }
    dlgGcodeProp dlg;

    dlg.exec();
    QString header = settings->value("header").toString();
    QString str;
    header.replace("$VERSION", "0.1");
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%m/%d/%Y %H:%M:%S",timeinfo);
    str=buffer;

    header.replace("$TIME", str);
    str.setNum(safez);

    header.replace("$SAFEZ", str);
    header.replace("$F", QString::number(feedspeed));
    header.replace("$P", QString::number(plungespeed));
    header.replace("$TS", QString::number(cutter_radius));
    header.replace("$TT", cutter_type);

    if (!header.endsWith("\n")) header.append("\n");
    QString metric = settings->value("metric").toString();
    if (!metric.endsWith("\n")) metric.append("\n");
    QString imperial = settings->value("imperial").toString();
    if (!imperial.endsWith("\n")) imperial.append("\n");

    QString preamble = settings->value("preamble").toString();
    if (!preamble.endsWith("\n")) preamble.append("\n");
    QString postamble = settings->value("postamble").toString();
    if (!postamble.endsWith("\n")) postamble.append("\n");
    QString beginpath = settings->value("beginpath").toString();
    if (!beginpath.endsWith("\n")) beginpath.append("\n");
    QString endpath = settings->value("endpath").toString();
    if (!endpath.endsWith("\n")) endpath.append("\n");
    QString beginrun = settings->value("beginrun").toString();
    if (!beginrun.endsWith("\n")) beginrun.append("\n");
    QString endrun = settings->value("endrun").toString();
    if (!endrun.endsWith("\n")) endrun.append("\n");
    QString point = settings->value("point").toString();
    QString slowline = settings->value("slowline").toString();
    QString firstlinepoint = settings->value("firstlinepoint").toString();
    QString arc = settings->value("arc").toString();
    QString start_spindle = settings->value("start_spindle").toString();
    QString stop_spindle = settings->value("stop_spindle").toString();
    QString beginline = settings->value("beginline").toString();
    QString endline = settings->value("endline").toString();
    gc.write(header.toUtf8());
    gc.write(preamble.toUtf8());
    gc.write(metric.toUtf8());
    gc.write(start_spindle.toUtf8());

    int slowlineflag=0;
    int beginpathflag=0;
    int beginrunflag=0;
    int beginlineflag=0;
    int endpathflag=0;
    int endrunflag=0;
    int endlineflag=0;
    unsigned long count=0;

    for(std::vector<Path*>::const_iterator p = paths.begin(); p!=paths.end(); ++p) {
        QString s;

        int rodd=0;
        for(std::vector<Run>::const_iterator r = (*p)->m_runs.begin(); r!=(*p)->m_runs.end(); ++r) {

                std::vector<Point>::const_iterator pt;


                if(rodd==0){
                    pt = r->m_points.begin();
                    int sz = r->m_points.size();
                    sz=0;
                    for(; pt!=r->m_points.end(); ++pt){
                        s=point;
                        if(dlg.ui->cbbeginpath->isChecked() && p == paths.begin() && r == (*p)->m_runs.begin() && pt == r->m_points.begin()){
//                            s=beginpath;
//                            replace(&s,*pt,(*p)->rot_x);
//                            gc.write(s.toUtf8());
                            beginpathflag=1;
                        }
                        if(dlg.ui->cbbeginrun->isChecked() && r == (*p)->m_runs.begin() && pt == r->m_points.begin()){
//                            s=beginrun;
//                            replace(&s,*pt,(*p)->rot_x);
//                            gc.write(s.toUtf8());
                            beginrunflag=2;
                        }
                        if(dlg.ui->cbbeginline->isChecked() && pt == r->m_points.begin()){
//                            s=beginline;
//                            replace(&s,*pt,(*p)->rot_x);
//                            gc.write(s.toUtf8());
                            beginlineflag=1;
                        }
                        if(dlg.ui->cbendline->isChecked() && pt == --r->m_points.end())endlineflag=1;
                        if(dlg.ui->cbendrun->isChecked() && r == --(*p)->m_runs.end() && pt == --r->m_points.end())endrunflag=1;
                        if(dlg.ui->cbendpath->isChecked() && p == --paths.end() &&  r == --(*p)->m_runs.end() && pt == --r->m_points.end())endpathflag=1;

                        if(pt->m_rad>0.0 && bEnableArcs)
                        {
                            s=arc;
                            if(pt->isCC)s.replace("G2", "G3");
                            s.replace("$R", QString::number(pt->m_rad,'f',3));
                            replace(&s,*pt,(*p)->rot_x);
                            gc.write(s.toUtf8());
                        }else
                        {
                            if(beginpathflag)
                            {
                                s=beginpath;
                                replace(&s,*pt,(*p)->rot_x);
                                gc.write(s.toUtf8());
                            }
                            if(beginrunflag)
                            {
                                if(beginrunflag==1)
                                {
                                    s=slowline;
                                    slowlineflag=1;
                                }
                                if(beginrunflag==2)s=beginrun;
                                replace(&s,*pt,(*p)->rot_x);
                                gc.write(s.toUtf8());
                            }
                            if(beginlineflag)
                            {
                                s=beginline;
                                replace(&s,*pt,(*p)->rot_x);
                                gc.write(s.toUtf8());
                            }
                            if(endlineflag && !slowlineflag){
                                s=endline;
                                replace(&s,*pt,(*p)->rot_x);
                                gc.write(s.toUtf8());
                            }
                            slowlineflag=0;
                            if(endrunflag){
                                s=endrun;
                                replace(&s,*pt,(*p)->rot_x);
                                gc.write(s.toUtf8());
                            }
                            if(endpathflag){
                                s=endpath;
                                replace(&s,*pt,(*p)->rot_x);
                                gc.write(s.toUtf8());
                            }
                            if(!beginpathflag && !beginrunflag && !beginlineflag && !endlineflag && !endrunflag && !endpathflag)
                            {
                                s=point;
                                replace(&s,*pt,(*p)->rot_x);
                                gc.write(s.toUtf8());
                            }
                            if(beginrunflag)beginrunflag--;
                            if(beginpathflag)beginpathflag=0;
                            if(beginlineflag)beginlineflag=0;
                            if(endlineflag)endlineflag=0;
                            if(endrunflag)endrunflag=0;
                            if(endpathflag)endpathflag=0;
                        }
                        count++;
                    }//for pt=+-
                }else{
                    Point pp;

                    pt = r->m_points.end();
                    long sz = r->m_points.size();
                    for(sz=r->m_points.size()-1; sz>=0; sz--)
                    {

                        pp = r->m_points[sz];
                        s = point;
                        if(dlg.ui->cbbeginpath->isChecked() && p == paths.begin() && r == (*p)->m_runs.begin() && sz == r->m_points.size()-1){
//                            s=beginpath;
//                            replace(&s,pp,(*p)->rot_x);
//                            gc.write(s.toUtf8());
                            beginpathflag=1;
                        }
                        if(dlg.ui->cbbeginrun->isChecked() && r == (*p)->m_runs.begin() && sz == r->m_points.size()-1){
//                            s=beginrun;
//                            replace(&s,pp,(*p)->rot_x);
//                            gc.write(s.toUtf8());
                            beginrunflag=2;
                        }
                        if(dlg.ui->cbbeginline->isChecked() && sz == r->m_points.size()-1){
//                            s=beginline;
//                            replace(&s,pp,(*p)->rot_x);
//                            gc.write(s.toUtf8());
                            beginlineflag=1;
                        }
                        if(dlg.ui->cbendline->isChecked() && sz == 0)endlineflag=1;
                        if(dlg.ui->cbendrun->isChecked() && r == --(*p)->m_runs.end() && sz == 0)endrunflag=1;
                        if(dlg.ui->cbendpath->isChecked() && p == --paths.end() && r == --(*p)->m_runs.end() && sz == 0)endpathflag=1;

                        if(pp.m_rad>0 && bEnableArcs)
                        {
                            s=arc;
                            if(pp.isCC)s.replace("G2", "G3");
                            s.replace("$R", QString::number(pp.m_rad,'f',3));
                            replace(&s,pp,(*p)->rot_x);
                            gc.write(s.toUtf8());
                        }else
                        {
                            s=point;
                            if(beginpathflag)
                            {
                                s=beginpath;
                                replace(&s,pp,(*p)->rot_x);
                                gc.write(s.toUtf8());
                            }
                            if(beginrunflag)
                            {
                                if(beginrunflag==1)
                                {
                                    s=slowline;
                                    slowlineflag=1;
                                }
                                if(beginrunflag==2)s=beginrun;
                                replace(&s,pp,(*p)->rot_x);
                                gc.write(s.toUtf8());
                            }
                            if(beginlineflag && !slowlineflag)
                            {
                                s=beginline;
                                replace(&s,pp,(*p)->rot_x);
                                gc.write(s.toUtf8());
                            }
                            slowlineflag=0;
                            if(endlineflag){
                                s=endline;
                                replace(&s,pp,(*p)->rot_x);
                                gc.write(s.toUtf8());
                            }
                            if(endrunflag){
                                s=endrun;
                                replace(&s,pp,(*p)->rot_x);
                                gc.write(s.toUtf8());
                            }
                            if(endpathflag){
                                s=endpath;
                                replace(&s,pp,(*p)->rot_x);
                                gc.write(s.toUtf8());
                            }
                            if(!beginpathflag && !beginrunflag && !beginlineflag  && !endlineflag && !endrunflag && !endpathflag)
                            {
                                s=point;
                                replace(&s,pp,(*p)->rot_x);
                                gc.write(s.toUtf8());
                            }
                            if(beginrunflag)beginrunflag--;
                            if(beginpathflag)beginpathflag=0;
                            if(beginlineflag)beginlineflag=0;
                            if(endlineflag)endlineflag=0;
                            if(endrunflag)endrunflag=0;
                            if(endpathflag)endpathflag=0;
                        }
                    count++;
                    }//for pt=+-
                }//else rodd==
                rodd=1-rodd;
        }//for m_runs iterator
    }//for paths iterator
    gc.write(stop_spindle.toUtf8());
    gc.write(postamble.toUtf8());
    gc.close();
    return true;
}

void GCodeExporter::replace(QString *s, Point pt, double ang)
{
    s->replace("$X", QString::number(pt.x(),'f',3));
    s->replace("$Y", QString::number(pt.y(),'f',3));
    s->replace("$Z", QString::number(pt.z(),'f',3));
    s->replace("$A", QString::number(ang,'f',3));
    s->replace("$SAFEZ", QString::number(safez,'f',3));
    s->replace("$F",QString::number(feedspeed,'f',3));
    s->replace("$P",QString::number(plungespeed,'f',3));
}
