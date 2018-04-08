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
#include "BaseCutter.h"

#include "intersection.h"

#include <QtOpenGL>
#include <GL/glu.h>

BaseCutter::BaseCutter(double radius, const Point& location)
    :Cutter(radius, location),m_cylinder(0)
{
    
}
 
BaseCutter::~BaseCutter()
{
    if (m_cylinder) {
        gluDeleteQuadric(m_cylinder);
    }
}
void 
BaseCutter::to_OpenGL()
{
    // NOT calling ::BaseCutter::to_OpenGL();
    glPushMatrix();
    glTranslatef(m_location.x(), m_location.y(), m_location.z());
    if (m_cylinder == NULL) {
        m_cylinder = gluNewQuadric();
    }
    gluCylinder(m_cylinder, m_radius, m_radius, m_height, 50, 1);
    glPopMatrix();
}

bool 
BaseCutter::intersect_cylinder_point(const Point& cl_in, const Point& direction, const Point& point,
                                            Point& cl_out, Point& ccp_out, double& d_out, Point& cp_out) const
{
    Point cl;
    Point ccp;
    Point cp;
    double d;
    if (Intersection::intersect_cylinder_point(cl_in, m_axis, m_radius, m_radiussq, direction, point,
                                             ccp, cp, d))
    {
        cl_out = cp.add(cl_in.sub(ccp));
        ccp_out = ccp;
        cp_out = cp;
        d_out = d;
        return true;
    }
    d_out = INFINITE;
    return false;
}

bool 
BaseCutter::intersect_cylinder_vertex(const Point& cl_in, const Point& direction, const Point& point,
                                             Point& cl_out, double& d_out, Point& cp_out) const
{
    Point cl;
    Point ccp;
    Point cp;
    double d;
    if (intersect_cylinder_point(cl_in, direction, point,
                                 cl, ccp, d, cp))
    {
        if (ccp.z() > cl_in.z() - epsilon) {
            cl_out = cl;
            cp_out = cp;
            d_out = d;
            return true;
        }
    }
    d_out = INFINITE;
    return false;
}

bool 
BaseCutter::intersect_cylinder_line(const Point& cl_in, const Point& direction, const Line& edge,
                                           Point& cl_out, Point& ccp_out, double& d_out, Point& cp_out) const
{
    Point cl;
    Point ccp;
    Point cp;
    double d;
    if (Intersection::intersect_cylinder_line(cl_in, m_axis, m_radius, m_radiussq, direction, edge,
                                              ccp, cp, d))
    {
        cl_out = cl_in.add(cp.sub(ccp));
        ccp_out = ccp;
        cp_out = cp;
        d_out = d;
        return true;
    }
    d_out = INFINITE;
    return false;
}

bool 
BaseCutter::intersect_cylinder_edge(const Point& cl_in, const Point& direction, const Line& edge,
                                           Point& cl_out, double& d_out, Point& cp_out) const
{
    Point cl;
    Point ccp;
    Point cp;
    double d;
    if (intersect_cylinder_line(cl_in, direction, edge,
                                cl, ccp, d, cp))
    {
        double m = cp.sub(edge.p1()).dot(edge.dir());
        if (m > -epsilon && m < edge.len() + epsilon) {
            if (ccp.z() > cl_in.z()) {
                cl_out = cl;
                cp_out = cp;
                d_out = d;
                return true;
            }
        }
    }
    d_out = INFINITE;
    return false;
}
