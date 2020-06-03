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
#include <QTextStream>
#include "SimpleCutter.h"
#include "PathProcessors.h"

bool SimpleCutter::GenerateCutPathx(const HeightField& heightfield, const Point& start, const Point& direction, double Zlevel, std::vector<Path*>& paths, double angle)
{
    Path* path = new Path();
    path->rot_x=angle;
    if (!GenerateCutPathLayer_y(heightfield, start, direction, Zlevel, *path)) {
        delete path;
        return false;
    }
    paths.push_back(path);
    return true;
}

bool
SimpleCutter::GenerateCutPath(const HeightField& heightfield, const Point& start, const Point& direction, const std::vector<double>& zlevels, std::vector<Path*>& paths, double angle)
{
    if (direction.z() != 0) return false;
    //if (direction.x() != 0 && direction.y() != 0) return false;
    if (direction.x() != 0 && direction.y() == 0) {
        for(size_t i=0; i<zlevels.size(); i++) {
            Path* path = new Path();
            path->rot_x=angle;
            if (!GenerateCutPathLayer_y(heightfield, start, direction, zlevels[zlevels.size()-i-1], *path)) {
                delete path;
                return false;
            }
            paths.push_back(path);
        }
        return true;
    }
    if (direction.x() == 0 && direction.y() != 0) {
        for(size_t i=0; i<zlevels.size(); i++) {
            Path* path = new Path();
            path->rot_x=angle;
            if (!GenerateCutPathLayer_x(heightfield, start, direction, zlevels[zlevels.size()-i-1], *path)) {
                delete path;
                return false;
            }
            paths.push_back(path);
        }
        return true;
    }
    if (direction.x() != 0 && direction.y() != 0) {
        for(size_t i=0; i<zlevels.size(); i++) {
            Path* path = new Path();
            path->rot_x=angle;
            if (!GenerateCutPathLayer_rect(heightfield, start, direction, zlevels[zlevels.size()-i-1], *path)) {
                delete path;
                return false;
            }
            paths.push_back(path);
        }
        return true;
    }
}

bool
SimpleCutter::GenerateCutPathLayer_x(const HeightField& heightfield, const Point& start, const Point& direction, double z_layer, Path& path)
{
    SimplePathProcessors pathProcessors;

    double xstep=m_stock->dim_x()/heightfield.width();
    double ystep=m_stock->dim_y()/heightfield.height();
    long di = 1+m_radius/xstep;
    long dj = 1+m_radius/ystep;
    long di1=0;
    long di2=0;
    long dj1=0;
    long dj2=0;


    for(size_t j=0; j<heightfield.width(); j+=(m_useLine)) {
        path.m_runs.resize(path.m_runs.size()+1);
        Run& run = path.m_runs.back();
//        size_t last_z = heightfield.height();
        for(size_t ii=0; ii<heightfield.height(); ii++) {
            size_t i = ii;
//            if (m_zigzag) i = heightfield.height()-ii;
            double z = 0.0;

            z = heightfield.point(j,i);

            di1=j-di;
            if(di1<0) di1=0;
            di2=j+di;
            if(di2>(heightfield.width()-1))di2=heightfield.width()-1;
            dj1=i-dj;
            if(dj1<0) dj1=0;
            dj2=i+dj;
            if(dj2>(heightfield.height()-1))dj2=heightfield.height()-1;
            double zav = 0.0;
            int zcnt = 0;
//            double zcc = -1000.0;

            for(long lj=dj1; lj<=dj2; lj++)
                for(long li=di1; li<=di2; li++)
                {
//                    double dis = sqrt(pow(heightfield.x(li)-heightfield.x(j),2)+pow(heightfield.y(lj)-heightfield.y(i),2));
//                    double zc = 0.0;
//                    if(!((li == j) && (lj == i)))
//                    {
//                        if(!m_isSpherical)
//                        {
//                            zc=heightfield.point(li,lj);
//                        }else
//                        {
//                            zc=heightfield.point(li,lj)-m_radius*cos((dis*3.1415926/(2.0*m_radius)));
//                        }
//                        if((dis<(m_radius+m_compmargin)) && (zc>zcc))
//                        {
//                            zcc=zc;
//                        }
//                    }
                    if((labs(li-(long)j)<=m_smooth) && (labs(lj-(long)i)<=m_smooth))
                    {
                        zav += heightfield.point(li,lj);
                        zcnt++;
                    }
                }//lj//li

            if(m_smooth && zcnt)
                z=zav/zcnt;

            if (z < z_layer) {
                z = z_layer;
            }
            z+=m_compmargin;
            run.m_points.push_back(Point(heightfield.x(j), heightfield.y(i), z));
        }
        pathProcessors.process(run.m_points);
    }
    return true;
}

bool
SimpleCutter::GenerateCutPathLayer_y(const HeightField& heightfield, const Point& start, const Point& direction, double z_layer, Path& path)
{
    SimplePathProcessors pathProcessors;
    double xstep=m_stock->dim_x()/heightfield.width();
    double ystep=m_stock->dim_y()/heightfield.height();
    long di = 1+m_radius/xstep;
    long dj = 1+m_radius/ystep;
    long di1=0;
    long di2=0;
    long dj1=0;
    long dj2=0;


    for(size_t i=0; i<heightfield.height(); i+=(m_useLine)) {
        path.m_runs.resize(path.m_runs.size()+1);
        Run& run = path.m_runs.back();
//        double last_z = 0;


/* ********************************************************************** */
        bool b_istraced=true;
        for(size_t jj=0; jj<heightfield.width(); jj++) {
            size_t j = jj;
//            if (m_zigzag) j = heightfield.width()-jj;

            double z = heightfield.point(j,i);


            di1=j-di;
            if(di1<0) di1=0;
            di2=j+di;
            if(di2>(heightfield.width()-1))di2=heightfield.width()-1;
            dj1=i-dj;
            if(dj1<0) dj1=0;
            dj2=i+dj;
            if(dj2>(heightfield.height()-1))dj2=heightfield.height()-1;
            double zav = 0.0;
            int zcnt = 0;
//            double zcc = -1000.0;

            for(long lj=dj1; lj<=dj2; lj++)
                for(long li=di1; li<=di2; li++)
                {
                    if((labs(li-(long)j)<=m_smooth) && (labs(lj-(long)i)<=m_smooth))
                    {
                        zav += heightfield.point(li,lj);
                        zcnt++;
                    }
                }//lj//li

            if(m_smooth && zcnt)
                z=zav/zcnt;

            if (z < z_layer) {
                z = z_layer;
                b_istraced=false;
            }else{

            }
            z+=m_compmargin;
            run.m_points.push_back(Point(heightfield.x(j), heightfield.y(i), z));
        }//j or jj

        pathProcessors.process(run.m_points);
    }//i
    return true;
}

bool
SimpleCutter::GenerateCutPathLayer_rect(const HeightField& heightfield, const Point& start, const Point& direction, double z_layer, Path& path)
{
    SimplePathProcessors pathProcessors;

    double xstep=m_stock->dim_x()/heightfield.width();
    double ystep=m_stock->dim_y()/heightfield.height();
    size_t di = 1+m_radius/xstep;
    size_t dj = 1+m_radius/ystep;
    long di1=0;
    long di2=0;
    long dj1=0;
    long dj2=0;
    int firstflag=1;
    Point p1=Point(0,0,0);
    Point p2=Point(0,0,0);
    Point p3=Point(0,0,0);
    Point p4=Point(0,0,0);
    Point pmem=Point(0,0,0);
    bool skipflag=false;

    path.m_runs.resize(path.m_runs.size()+1);
    Run& run = path.m_runs.back();
    QFile logfile("/home/fred/logfile.txt");
//    logfile.open(QIODevice::WriteOnly | QIODevice::Append);
    logfile.open(QIODevice::WriteOnly);
    QTextStream qts(&logfile);

    size_t i=0;
    size_t j=0;
    size_t pos=0;
newpos:
    double z = heightfield.point(i,j);
    if(z_layer > z) z=z_layer;

    z+=m_compmargin;
    p4=p3;
    p3=p2;
    p2=p1;
    p1=Point(heightfield.x(i), heightfield.y(j), z);
    if((p2.x()==p1.x() && p2.y()==p1.y() && p2.z()==p1.z())
            || (p3.x()==p1.x() && p3.y()==p1.y() && p3.z()==p1.z())
                || (p4.x()==p1.x() && p4.y()==p1.y() && p4.z()==p1.z()))
    {
        goto myexi;
    }
    if(p1.x()==pmem.x() && p1.z()==pmem.z() && p1.y()!=pmem.y())
    {
        pmem=p1;
        skipflag=true;
    }else if(p1.y()==pmem.y() && p1.z()==pmem.z() && p1.x()!=pmem.x())
    {
        pmem=p1;
        skipflag=true;
    }else
    {
        if(skipflag)
        {
            skipflag=false;
            run.m_points.push_back(pmem);
        }
        run.m_points.push_back(p1);
    }
    qts << i << "," << j << "," << z << "\n";

    if(j==pos && i==pos && firstflag==0)
    {
        pos++;
        i++;
        j++;
        if(pos>(heightfield.width()/2+1) || (pos>(heightfield.height()/2+1)))
        {
            qts << "endofrun " << i << "," << j << "," << z << "\n";
            goto myexi;
        }
        firstflag=1;
        goto newpos;
    }
    if(i<(heightfield.width()-pos-1) && j==pos)
    {
        i++;
        firstflag=0;
        goto newpos;
    }else if(i==(heightfield.width()-pos-1) && j<(heightfield.height()-pos-1))
    {
        j++;
        goto newpos;
    }else if(j==(heightfield.height()-pos-1) && (i > pos))
    {
        i--;
        goto newpos;
    }else if(i == pos && j > pos)
    {
        j--;
        goto newpos;
    }

myexi:
    pathProcessors.process(run.m_points);
    return true;
}




//    di1=i-di;
//    if(di1<0) di1=0;
//    di2=i+di;
//    if(di2>(size_t)(heightfield.width()-1))di2=heightfield.width()-1;
//    dj1=j-dj;
//    if(dj1<0) dj1=0;
//    dj2=j+dj;
//    if(dj2>(heightfield.height()-1))dj2=heightfield.height()-1;
//    double zav = 0.0;
//    int zcnt = 0;
//    double zcc = -1000.0;

//    for(long lj=dj1; lj<=dj2; lj++)
//        for(long li=di1; li<=di2; li++)
//        {
//            double dis = sqrt(pow(heightfield.x(li)-heightfield.x(j),2)+pow(heightfield.y(lj)-heightfield.y(i),2));
//            double zc = -1000.0;
//            if(!((li == j) && (lj == i)))
//            {
//                if(!m_isSpherical)
//                {
//                    zc=heightfield.point(li,lj);
//                }else
//                {
//                    zc=heightfield.point(li,lj)-m_radius*sin((dis*3.1415926/(2.0*m_radius)));
//                }
//                if((dis<(m_radius+m_compmargin)) && (zc>zcc))
//                {
//                    zcc=zc;
//                }
//            }
//            if((labs(li-(long)j)<=m_smooth) && (labs(lj-(long)i)<=m_smooth))
//            {
//                zav += heightfield.point(li,lj);
//                zcnt++;
//            }
//        }//lj//li

//    if(m_smooth && zcnt)
//        z=zav/zcnt;

//    if(zcc>z)z=zcc;
//    if (z < z_layer) {
//        z = z_layer;
//    }
