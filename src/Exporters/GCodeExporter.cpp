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
#include "GCodeExporter.h"

#include <QFile>
#include <QSettings>

QSettings* settings = NULL;

GCodeExporter::GCodeExporter()
{
    if (settings == NULL) {
        settings = new QSettings("/data/projects/cppcam/config/GCode.conf", QSettings::IniFormat);
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
    header.replace("$F", QString::number(feedspeed));
    header.replace("$P", QString::number(plungespeed));

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
    QString firstlinepoint = settings->value("firstlinepoint").toString();
    QString arc = settings->value("arc").toString();
    QString start_spindle = settings->value("start_spindle").toString();
    QString stop_spindle = settings->value("stop_spindle").toString();
    gc.write(header.toUtf8());
    gc.write(preamble.toUtf8());
    gc.write(metric.toUtf8());
    gc.write(start_spindle.toUtf8());
    bool flag_beginpath=1;
    for(std::vector<Path*>::const_iterator p = paths.begin(); p!=paths.end(); ++p) {
        flag_beginpath=true;
        QString s;
        std::vector<Point>::const_iterator pt;
        Point pp;

        bool flag_firstlinepoint=true;
        if(m_rectcut)flag_firstlinepoint=false;
        int rodd=0;
        for(std::vector<Run>::const_iterator r = (*p)->m_runs.begin(); r!=(*p)->m_runs.end(); ++r) {

                bool flag_beginrun=true;
                std::vector<Point>::const_iterator pt;
                int sz = r->m_points.size();


                if(rodd==0){
                    pt = r->m_points.begin();
                    sz=0;
                    for(; pt!=r->m_points.end(); ++pt){
                        pp=r->m_points[sz];
                        if(flag_beginpath){
                            flag_beginpath=false;
                            s=beginpath;
                            s.replace("$X", QString::number(pt->x(),'f',3));
                            s.replace("$Y", QString::number(pt->y(),'f',3));
                            s.replace("$Z", QString::number(pt->z(),'f',3));
                            s.replace("$A", QString::number((*p)->rot_x,'f',3));
                            s.replace("$SAFEZ", QString::number(safez,'f',3));
                            s.replace("$F",QString::number(feedspeed,'f',3));
                            s.replace("$P",QString::number(plungespeed,'f',3));
                            gc.write(s.toUtf8());
                        }
                        if(flag_beginrun){
                            flag_beginrun=false;
                            s=beginrun;
                            s.replace("$X", QString::number(pt->x(),'f',3));
                            s.replace("$Y", QString::number(pt->y(),'f',3));
                            s.replace("$Z", QString::number(pt->z(),'f',3));
                            s.replace("$A", QString::number((*p)->rot_x,'f',3));
                            s.replace("$SAFEZ", QString::number(safez,'f',3));
                            s.replace("$F",QString::number(feedspeed,'f',3));
                            s.replace("$P",QString::number(plungespeed,'f',3));
                            gc.write(s.toUtf8());
                        }
                        if(pt->m_rad>0.0 && bEnableArcs){
                            s=arc;
                            if(pt->isCC)s.replace("G2", "G3");
                            s.replace("$R", QString::number(pt->m_rad,'f',3));
                        }else{
                            if(flag_firstlinepoint)
                            {
                                s=firstlinepoint;
                            }else
                            {
                                s=point;
                            }
                        }
                        s.replace("$X", QString::number(pt->x(),'f',3));
                        s.replace("$Y", QString::number(pt->y(),'f',3));
                        s.replace("$Z", QString::number(pt->z(),'f',3));
                        s.replace("$SAFEZ", QString::number(safez,'f',3));
                        s.replace("$F",QString::number(feedspeed,'f',3));
                        s.replace("$P",QString::number(plungespeed,'f',3));
                        gc.write(s.toUtf8());
                        sz++;
                    }//for pt=+-
                }else{
                    pt = r->m_points.end();
                    pp = r->m_points[sz-1];
                    for(sz=r->m_points.size()-1; sz>=0; sz--) {
                        pp=r->m_points[sz];
                        if(flag_beginpath){
                            flag_beginpath=false;
                            s=beginpath;
                            s.replace("$X", QString::number(pp.m_x,'f',3));
                            s.replace("$Y", QString::number(pp.m_y,'f',3));
                            s.replace("$Z", QString::number(pp.m_z,'f',3));
                            s.replace("$A", QString::number((*p)->rot_x,'f',3));
                            s.replace("$SAFEZ", QString::number(safez,'f',3));
                            s.replace("$F",QString::number(feedspeed,'f',3));
                            s.replace("$P",QString::number(plungespeed,'f',3));
                            gc.write(s.toUtf8());
                        }
                        if(flag_beginrun){
                            flag_beginrun=false;
                            s=beginrun;
                            s.replace("$X", QString::number(pp.m_x,'f',3));
                            s.replace("$Y", QString::number(pp.m_y,'f',3));
                            s.replace("$Z", QString::number(pp.m_z,'f',3));
                            s.replace("$A", QString::number((*p)->rot_x,'f',3));
                            s.replace("$SAFEZ", QString::number(safez,'f',3));
                            s.replace("$F",QString::number(feedspeed,'f',3));
                            s.replace("$P",QString::number(plungespeed,'f',3));
                            gc.write(s.toUtf8());
                        }
                        if(pt->m_rad>0 && bEnableArcs){
                            s=arc;
                            if(pt->isCC)s.replace("G2", "G3");
                            s.replace("$R", QString::number(pt->m_rad,'f',3));
                        }else{
                            if(flag_firstlinepoint)
                            {
                                s=firstlinepoint;
                            }else
                            {
                                s=point;
                            }
                        }
                        s.replace("$X", QString::number(pp.m_x,'f',3));
                        s.replace("$Y", QString::number(pp.m_y,'f',3));
                        s.replace("$Z", QString::number(pp.m_z,'f',3));
                        s.replace("$SAFEZ", QString::number(safez,'f',3));
                        s.replace("$F",QString::number(feedspeed,'f',3));
                        s.replace("$P",QString::number(plungespeed,'f',3));
                        gc.write(s.toUtf8());
                    }//for pt=+-
                }//else

                flag_firstlinepoint=false;
                rodd=1-rodd;
                //const Point* pt = &r->m_points.back();
                s = endrun;
                s.replace("$X", QString::number(pp.m_x,'f',3));
                s.replace("$Y", QString::number(pp.m_y,'f',3));
                s.replace("$Z", QString::number(pp.m_z,'f',3));
                s.replace("$SAFEZ", QString::number(safez,'f',3));
                s.replace("$F",QString::number(feedspeed,'f',3));
                s.replace("$P",QString::number(plungespeed,'f',3));
                gc.write(s.toUtf8());
        }//for m_runs iterator
        s = endpath;
        s.replace("$X", QString::number(pp.m_x,'f',3));
        s.replace("$Y", QString::number(pp.m_y,'f',3));
        s.replace("$Z", QString::number(pp.m_z,'f',3));
        s.replace("$SAFEZ", QString::number(safez,'f',3));
        s.replace("$F",QString::number(feedspeed,'f',3));
        s.replace("$P",QString::number(plungespeed,'f',3));
        gc.write(s.toUtf8());
    }//for paths iterator
    gc.write(stop_spindle.toUtf8());
    gc.write(postamble.toUtf8());
    gc.close();
    return true;
}
