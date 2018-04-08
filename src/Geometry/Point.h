/* -*- coding: utf-8 -*-

Copyright 2008-2014 Lode Leroy

This file is part of PyCAM.

PyCAM is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

PyCAM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with PyCAM.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __POINT_H
#define __POINT_H

#include <cmath>

struct Point
{
    Point(double x=0, double y=0, double z=0) : m_x(x),m_y(y),m_z(z) { }

    double m_x=0.0;
    double m_y=0.0;
    double m_z=0.0;
    double m_rad=-1;
    bool isCC=false;

    double x() const { return m_x; }
    double y() const { return m_y; }
    double z() const { return m_z; }

    double normsq() const { return normsq(*this); }
    double norm() const { return norm(*this); }
    Point& normalize() { return normalize(*this); }
    Point normalized() const { return normalized(*this); }
    Point mul(double f) const { return mul(*this, f); }
    Point div(double f) const { return div(*this, f); }
    Point& divide(double f)  { return divide(*this, f); }

    double dot(const Point& other) const { return dot(*this, other); }
    Point mid(const Point& other) const { return mid(*this, other); }
    Point sub(const Point& other) const { return sub(*this, other); }
    Point add(const Point& other) const { return add(*this, other); }
    Point cross(const Point& other) const { return cross(*this, other); }

    double dot(const Point* other) const { return dot(*this, *other); }
    Point mid(const Point* other) const { return mid(*this, *other); }
    Point sub(const Point* other) const { return sub(*this, *other); }
    Point add(const Point* other) const { return add(*this, *other); }
    Point cross(const Point* other) const { return cross(*this, *other); }

    static Point mid(const Point* p1, const Point* p2) { return mid(*p1, *p2); }
    static Point mid(const Point* p1, const Point* p2, const Point* p3) { return mid(*p1, *p2, *p3); }
    static Point sub(const Point* p1, const Point* p2) { return sub(*p1, *p2); }
    static Point add(const Point* p1, const Point* p2) { return add(*p1, *p2); }
    static Point cross(const Point* p1, const Point* p2) { return cross(*p1, *p2); }
    static Point cross_normalized(const Point* p1, const Point* p2) { return cross_normalized(*p1, *p2); }

    static Point mul(const Point& p1, double f) { return Point(p1.x()*f,p1.y()*f,p1.z()*f); }
    static Point div(const Point& p1, double f) { return Point(p1.x()/f,p1.y()/f,p1.z()/f); }
    static double dot(const Point& p1, const Point& p2) { return p1.x()*p2.x()+p1.y()*p2.y()+p1.z()*p2.z(); }
    static Point mid(const Point& p1, const Point& p2) { return Point((p1.x()+p2.x())/2,(p1.y()+p2.y())/2,(p1.z()+p2.z())/2); }
    static Point mid(const Point& p1, const Point& p2, const Point& p3) { return Point((p1.x()+p2.x()+p3.x())/3,(p1.y()+p2.y()+p3.y())/3,(p1.z()+p2.z()+p3.z())/3); }
    static Point sub(const Point& p1, const Point& p2) { return Point(p1.x()-p2.x(),p1.y()-p2.y(),p1.z()-p2.z()); }
    static Point add(const Point& p1, const Point& p2) { return Point(p1.x()+p2.x(),p1.y()+p2.y(),p1.z()+p2.z()); }
    static double normsq(const Point& p1) { return p1.x()*p1.x()+p1.y()*p1.y()+p1.z()*p1.z(); }
    static double norm(const Point& p1) { double normsq = p1.normsq(); if (normsq>0) return sqrt(normsq); return 0; }
    static Point cross(const Point& p1, const Point& p2) { return Point((p1.y())*(p2.z())-(p2.y())*(p1.z()),(p2.x())*(p1.z())-(p1.x())*(p2.z()),(p1.x())*(p2.y())-(p2.x())*(p1.y())); }
    static Point normalized(const Point& p1) { Point p(p1); normalize(p); return p; }
    static Point cross_normalized(const Point& p1, const Point& p2) { return normalized(cross(p1,p2)); }

    static Point& divide(Point& p1, double f) { p1=Point(p1.x()/f,p1.y()/f,p1.z()/f); return p1; }
    static Point& normalize(Point& p1) { double normsq = p1.normsq(); if (normsq>0) { Point::divide(p1, sqrt(normsq)); } return p1; }
};

#endif
