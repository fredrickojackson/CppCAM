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
#include <QDateTime>

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
GCodeExporter::ExportPath(const std::vector<Path*> paths, QString filename)
{
    long szpaths=0;
    long szruns=0;
    long szpoints=0;
    long iszpaths=0;
    long iszruns=0;
    long iszpoints=0;
    Point pttmp;
    dlgGcodeProp dlg;
    QString filenameOrig=filename;
    dlg.exec();
    if (dlg.result() == QDialog::Accepted)
    {
        QString header = settings->value("header").toString();
        QString str;
        header.replace("$VERSION", "0.9");
        time_t rawtime;
        struct tm * timeinfo;
        char buffer[80];
        QDateTime qdt;
        str=qdt.currentDateTime().toString();
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
        if(m_radial)
            header.replace("$ZOFF",QString::number(-(mdl->max_z()+mdl->min_z())/2.0 ));
        else
            header.replace("$ZOFF",QString::number(0.00));


        if (!header.endsWith("\n")) header.append("\n");
        QString metric = settings->value("metric").toString();
        if (!metric.endsWith("\n")) metric.append("\n");
        QString imperial = settings->value("imperial").toString();
        if (!imperial.endsWith("\n")) imperial.append("\n");

        QString preamble = settings->value("preamble").toString();
        if (!preamble.endsWith("\n")) preamble.append("\n");
        preamble.replace("$MXMIN",QString::number(mdl->min_x()));
        preamble.replace("$MXMAX",QString::number(mdl->max_x()));
        preamble.replace("$MXDIM",QString::number(mdl->dim_x()));
        preamble.replace("$MYMIN",QString::number(mdl->min_y()));
        preamble.replace("$MYMAX",QString::number(mdl->max_y()));
        preamble.replace("$MYDIM",QString::number(mdl->dim_y()));
        preamble.replace("$MZMIN",QString::number(mdl->min_z()));
        preamble.replace("$MZMAX",QString::number(mdl->max_z()));
        preamble.replace("$MZDIM",QString::number(mdl->dim_z()));
        preamble.replace("$XMIN",QString::number(stk->min_x()));
        preamble.replace("$XMAX",QString::number(stk->max_x()));
        preamble.replace("$XDIM",QString::number(stk->dim_x()));
        preamble.replace("$YMIN",QString::number(stk->min_y()));
        preamble.replace("$YMAX",QString::number(stk->max_y()));
        preamble.replace("$YDIM",QString::number(stk->dim_y()));
        preamble.replace("$ZMIN",QString::number(stk->min_z()));
        preamble.replace("$ZMAX",QString::number(stk->max_z()));
        preamble.replace("$ZDIM",QString::number(stk->dim_z()));

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

        int beginpathflag=0;
        int beginrunflag=0;
        int beginlineflag=0;
        int endpathflag=0;
        int endrunflag=0;
        int endlineflag=0;
        int slowlineflag=0;
        unsigned long count=0;
        szpaths=paths.size();
        iszpaths=0;

        QStringList files;
        for(std::vector<Path*>::const_iterator p = paths.begin(); p!=paths.end(); ++p) {
            QString fn=filenameOrig;
            if((*p)->qlrunindex==0)
                fn=filenameOrig;
            else
                fn=fn.replace(".",QString::number((*p)->qlrunindex).append("."));
            files.append(fn);
            QFile gc(fn);
            gc.remove();
            iszpaths++;
        }
        files.removeDuplicates();
        for(int ifn=0;ifn<files.size();ifn++)
        {
            QFile gc(files.at(ifn));
            gc.open(QIODevice::WriteOnly | QIODevice::NewOnly);
            gc.write(header.toUtf8());
            gc.write(preamble.toUtf8());
            gc.write(metric.toUtf8());
            gc.write(start_spindle.toUtf8());
            gc.close();
        }

        iszpaths=0;
        for(std::vector<Path*>::const_iterator p = paths.begin(); p!=paths.end(); ++p) {
            QString fn=files.at((*p)->qlrunindex);
            QFile gc(fn);
            gc.open(QIODevice::WriteOnly | QIODevice::Append);

            if(dlg.ui->cbbeginpath->isChecked())beginpathflag=1;
            QString s;
            int rodd=0;
            szruns=(*p)->m_runs.size();
            iszruns=0;
            for(std::vector<Run>::const_iterator r = (*p)->m_runs.begin(); r!=(*p)->m_runs.end(); ++r) {
                if(dlg.ui->cbbeginrun->isChecked())beginrunflag=1;
                cutter_type= (*p)->tooltype;
                cutter_radius=(*p)->toolradius;

                if(rodd==0){
                    szpoints=r->m_points.size();
                    iszpoints=0;
                    for(std::vector<Point>::const_iterator pt = r->m_points.begin(); pt!=r->m_points.end(); ++pt){
                        s=point;
                        if(dlg.ui->cbbeginline->isChecked() && pt == r->m_points.begin())
                            beginlineflag=1;
                        else
                            beginlineflag=0;
                        if(dlg.ui->cbendline->isChecked() && pt == --r->m_points.end())
                            endlineflag=1;
                        else
                            endlineflag=0;

                        if(dlg.ui->cbslowline->isChecked() && beginlineflag && beginrunflag && beginpathflag)
                        {
                            slowlineflag=2;
                        }

                        if(pt->m_rad>0.0 && bEnableArcs)
                        {
                            s=arc;
                            if(pt->isCC)s.replace("G2", "G3");
                            s.replace("$R", QString::number(pt->m_rad,'f',3));
                            replace(&s,*pt,(*p)->rot_x);
                            gc.write(s.toUtf8());
                        }else{

                            if(!beginpathflag && !beginrunflag && !beginlineflag && !endlineflag && !endrunflag && !endpathflag)
                            {
                                s=point;
                                replace(&s,*pt,(*p)->rot_x);
                                gc.write(s.toUtf8());
                            }
                            if(beginpathflag)
                            {
                                s=beginpath;
                                replace(&s,*pt,(*p)->rot_x);
                                gc.write(s.toUtf8());
                                beginpathflag=0;
                                if(dlg.ui->cbExclusive->isChecked())
                                {
                                    beginrunflag=0;
                                    beginlineflag=0;
                                }
                            }
                            if(beginrunflag)
                            {
                                s=beginrun;
                                replace(&s,*pt,(*p)->rot_x);
                                gc.write(s.toUtf8());
                                beginrunflag=0;
                                if(dlg.ui->cbExclusive->isChecked())
                                {
                                    beginlineflag=0;
                                }
                            }
                            if(beginlineflag)
                            {
                                s=beginline;
                                replace(&s,*pt,(*p)->rot_x);
                                gc.write(s.toUtf8());
                                beginlineflag=0;
                            }
                            if(((!(!dlg.ui->cbendrun->isChecked() && !endlineflag && slowlineflag && !(pt == r->m_points.begin()))) && (endlineflag && !dlg.ui->cbExclusive->isChecked())) || (dlg.ui->cbExclusive->isChecked() && !dlg.ui->cbendrun->isChecked() && endlineflag && slowlineflag))
                            {
                                s=slowline;
                                replace(&s,*pt,(*p)->rot_x);
                                gc.write(s.toUtf8());
                                if(dlg.ui->cbExclusive->isChecked())endlineflag=0;
                            }
                            if((endlineflag && !dlg.ui->cbExclusive) || (dlg.ui->cbExclusive->isChecked() && !dlg.ui->cbendrun->isChecked() && endlineflag))
                            {
                                s=endline;
                                replace(&s,*pt,(*p)->rot_x);
                                gc.write(s.toUtf8());
                                endlineflag=0;
                            }
                        }//if(pt->m_rad>0.0 && bEnableArcs)
                        if(pt == --r->m_points.end())
                            slowlineflag=0;
                        count++;
                        iszpoints++;
                        pttmp=*pt;
                    }//for pt=+-
                }else{ //rodd !== 0
                    szpoints=r->m_points.size();
                    iszpoints=0;
                    for(std::vector<Point>::const_reverse_iterator pt = r->m_points.rbegin(); pt!=r->m_points.rend(); ++pt){
                        s=point;
                        if(dlg.ui->cbbeginline->isChecked() && pt == r->m_points.rbegin())
                            beginlineflag=1;
                        else
                            beginlineflag=0;
                        if(dlg.ui->cbendline->isChecked() && pt == --r->m_points.rend())
                            endlineflag=1;
                        else
                            endlineflag=0;

                        if(dlg.ui->cbslowline->isChecked() && beginlineflag && beginrunflag && beginpathflag)
                        {
                            slowlineflag=2;
                        }

                        if(pt->m_rad>0.0 && bEnableArcs)
                        {
                            s=arc;
                            if(pt->isCC)s.replace("G2", "G3");
                            s.replace("$R", QString::number(pt->m_rad,'f',3));
                            replace(&s,*pt,(*p)->rot_x);
                            gc.write(s.toUtf8());
                        }else
                        {
                            if(!beginpathflag && !beginrunflag && !beginlineflag && !endlineflag && !endrunflag && !endpathflag)
                            {
                                s=point;
                                replace(&s,*pt,(*p)->rot_x);
                                gc.write(s.toUtf8());
                            }
                            if(beginpathflag)
                            {
                                s=beginpath;
                                replace(&s,*pt,(*p)->rot_x);
                                gc.write(s.toUtf8());
                                beginpathflag=0;
                                if(dlg.ui->cbExclusive->isChecked())
                                {
                                    beginrunflag=0;
                                    beginlineflag=0;
                                }
                            }
                            if(beginrunflag)
                            {
                                s=beginrun;
                                replace(&s,*pt,(*p)->rot_x);
                                gc.write(s.toUtf8());
                                beginrunflag=0;
                                if(dlg.ui->cbExclusive->isChecked())
                                {
                                    beginlineflag=0;
                                }
                            }
                            if(beginlineflag)
                            {
                                s=beginline;
                                replace(&s,*pt,(*p)->rot_x);
                                gc.write(s.toUtf8());
                                beginlineflag=0;
                            }
                            if(((!(!dlg.ui->cbendrun->isChecked() && !endlineflag && slowlineflag && !(pt == r->m_points.rbegin()))) && (endlineflag && !dlg.ui->cbExclusive->isChecked())) || (dlg.ui->cbExclusive->isChecked() && !dlg.ui->cbendrun->isChecked() && endlineflag && slowlineflag))
                            {
                                    s=slowline;
                                    replace(&s,*pt,(*p)->rot_x);
                                    gc.write(s.toUtf8());
                                    if(dlg.ui->cbExclusive->isChecked())endlineflag=0;
                            }
                            if((endlineflag && !dlg.ui->cbExclusive) || (dlg.ui->cbExclusive->isChecked() && !dlg.ui->cbendrun->isChecked() && endlineflag))
                            {
                                s=endline;
                                replace(&s,*pt,(*p)->rot_x);
                                gc.write(s.toUtf8());
                                endlineflag=0;
                            }
                        }//if(pt->m_rad>0.0 && bEnableArcs)
                        if(pt == --r->m_points.rend())
                            slowlineflag=0;
                        count++;
                        iszpoints++;
                        pttmp=*pt;
                    }//for pt=+-
                }//else rodd==
                rodd=1-rodd;
                gc.flush();
                iszruns++;
                if(dlg.ui->cbendrun->isChecked())
                {
                    s=endrun;
                    replace(&s,pttmp,(*p)->rot_x);
                    gc.write(s.toUtf8());
                    endrunflag=0;
                }
            }//for m_runs iterator
            iszpaths++;
            if(dlg.ui->cbendpath->isChecked())
            {
                s=endpath;
                replace(&s,pttmp,(*p)->rot_x);
                gc.write(s.toUtf8());
                endpathflag=0;
            }
            gc.close();
        }//for paths iterator

        for(int ifn=0;ifn<files.size();ifn++)
        {
            QFile gc(files.at(ifn));
            gc.open(QIODevice::WriteOnly | QIODevice::Append);
            gc.write(stop_spindle.toUtf8());
            gc.write(postamble.toUtf8());
            gc.close();
        }
        return true;
    }else
    {
        return false;
    }
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
    s->replace("$DRLTYPE",cutter_type);
    s->replace("$DRLRADIUS",QString::number(cutter_radius,'f',3));
}
