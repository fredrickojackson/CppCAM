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
#include "Cutter.h"

#include "SphericalCutter.h"
#include "CylindricalCutter.h"

#include "intersection.h"

Cutter::Cutter(double radius, const Point& location)
    :m_radius(radius),m_location(location),m_height(10),m_axis(Point(0,0,1)),m_radiussq(sqr(m_radius))
{
}


Cutter* Cutter::CreateSphericalCutter(double radius, const Point& location)
{
    return new SphericalCutter(radius, location);
}

Cutter* Cutter::CreateCylindricalCutter(double radius, const Point& location)
{
    return new CylindricalCutter(radius, location);
}

bool 
Cutter::intersect(const Point& cl_in, const Point& direction, const Model& model, Point& cl_out, double& d_out, Point& cp_out) const
{
    d_out = INFINITE;
    for(size_t i=0; i<model.m_triangles.size(); i++) {
        const Triangle& triangle = model.m_triangles[i];
        Point cl;
        double d;
        Point cp;
        if (intersect(cl_in, direction, triangle, cl, d, cp)) {
            if (d < d_out) {
                cl_out = cl;
                cp_out = cp;
                d_out = d;
            }
        }
    }
    return d_out < INFINITE;
}

bool 
Cutter::drop(const Point& cl_in, const Triangle& triangle, Point& cl_out) const
{
    if (min_x(cl_in) > triangle.max_x() + epsilon) {
        return false;
    }
    if (min_y(cl_in) > triangle.max_y() + epsilon) {
        return false;
    }
    if (max_x(cl_in) < triangle.min_x() - epsilon) {
        return false;
    }
    if (max_y(cl_in) < triangle.min_y() - epsilon) {
        return false;
    }
#if 0
    Point c = triangle.center();
    if (sqr(c.x()-cl_in.x())+sqr(c.y()-cl_in.y())> sqr(radius() + triangle.radius()) + epsilon)
        return false;
#endif
    const Point vertical = Point(0,0,-1);
    double d;
    Point cp;
    Point cl;
    if(intersect(cl_in, vertical, triangle, cl, d, cp)) {
        cl_out = cl;
        return true;
    }
    return false;
}

Point& 
Cutter::drop(const Triangle& triangle)
{
    Point cl;
    if (drop(m_location, triangle, cl)) {
        m_location = cl;
    }
    return m_location;
}

bool 
Cutter::drop(const Point& cl_in, const Model& model, Point& cl_out) const
{
    double z = -INFINITE;
    for(size_t i=0; i<model.m_triangles.size(); i++) {
        const Triangle& triangle = model.m_triangles[i];
        if (z < triangle.max_z() + epsilon) 
        {
            Point cl;
            if (drop(cl_in, triangle, cl)) {
                if (cl.z() > z) {
                    cl_out = cl;
                    z = cl.z();
                }
            }
        }
    }
    return z > -INFINITE;
}

Point& 
Cutter::drop(const Model& model)
{
    Point cl;
    if (drop(m_location, model, cl)) {
        m_location = cl;
    }
    return m_location;
}

bool 
Cutter::push(const Point& cl_in, const Point& direction, const Triangle& triangle, Point& cl_out) 
{
    Point cl;
    double d;
    Point cp;
    if (direction.x() == 0) {
        if (max_x(cl_in) + epsilon < triangle.min_x()) {
            return false;
        }
        if (min_x(cl_in) - epsilon > triangle.max_x()) {
            return false;
        }
    }
    if (direction.y() == 0) {
        if (max_y(cl_in) + epsilon < triangle.min_y()) {
            return false;
        }
        if (min_y(cl_in) - epsilon > triangle.max_y()) {
            return false;
        }
    }

    if (intersect(cl_in, direction, triangle, cl, d, cp))
    {
        cl_out = cl;
        return true;
    }
    return false;
}

Point& 
Cutter::push(const Point& direction, const Triangle& triangle) 
{
    Point cl;
    if (push(m_location, direction, triangle, cl)) {
        m_location = cl;
    }
    return m_location;
}

bool 
Cutter::push(const Point& cl_in, const Point& direction, const Model& model, Point& cl_out) 
{
    Point cl;
    double d;
    Point cp;
    if (intersect(cl_in, direction, model, cl, d, cp))
    {
        cl_out = cl;
        return true;
    }
    return false;
}

Point& 
Cutter::push(const Point& direction, const Model& model) 
{
    Point cl;
    if (push(m_location, direction, model, cl)) {
        m_location = cl;
    }
    return m_location;
}

