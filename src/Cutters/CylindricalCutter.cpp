/* -*- coding: utf-8 -*-

Copyright 2008-2014 Lode Leroy

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

#include "CylindricalCutter.h"

#include "intersection.h"

#include <QtOpenGL>
#include <GL/glu.h>

CylindricalCutter::CylindricalCutter(double radius, const Point& location)
    :BaseCutter(radius, location),m_disk(0) 
{
    
}
 
CylindricalCutter::~CylindricalCutter()
{
    if (m_disk) {
        gluDeleteQuadric(m_disk);
    }
}

void 
CylindricalCutter::to_OpenGL()
{
    // NOT calling ::BaseCutter::to_OpenGL();
    glPushMatrix();
    glTranslatef(m_location.x(), m_location.y(), m_location.z());
    if (m_cylinder == NULL) {
        m_cylinder = gluNewQuadric();
    }
    gluCylinder(m_cylinder, m_radius, m_radius, m_height, 50, 1);
    if (m_disk == NULL) {
        m_disk = gluNewQuadric();
    }
    gluDisk(m_disk, 0, m_radius, 50, 1);
    glPopMatrix();
}

bool 
CylindricalCutter::intersect_circle_plane(const Point& cl_in, const Point& direction, const Triangle& triangle,
                                          Point& cl_out, Point& ccp_out, double& d_out, Point& cp_out) const
{
    Point cl;
    Point ccp;
    Point cp;
    double d;
    if (Intersection::intersect_circle_plane(cl_in, m_radius, direction, Plane(triangle.p1(), triangle.n()),
                                             ccp, cp, d))
    {
        cl_out = cp.add(cl_in.sub(ccp));
        ccp_out = ccp;
        d_out = d;
        cp_out = cp;
        return true;
    }
    d_out = INFINITE;
    return false;
}

bool 
CylindricalCutter::intersect_circle_point(const Point& cl_in, const Point& direction, const Point& point,
                                          Point& cl_out, Point& ccp_out, double& d_out, Point& cp_out) const
{
    Point cl;
    Point ccp;
    Point cp;
    double d;
    if (Intersection::intersect_circle_point(cl_in, m_axis, m_radius, m_radiussq, direction, point,
                                             ccp, cp, d))
    {
        cl_out = cp.add(cl_in.sub(ccp));
        ccp_out = ccp;
        d_out = d;
        cp_out = cp;
        return true;
    }
    d_out = INFINITE;
    return false;
}

bool 
CylindricalCutter::intersect_circle_line(const Point& cl_in, const Point& direction, const Line& edge,
                                         Point& cl_out, Point& ccp_out, double& d_out, Point& cp_out) const
{
    Point cl;
    Point ccp;
    Point cp;
    double d;
    if (Intersection::intersect_circle_line(cl_in, m_axis, m_radius, m_radiussq, direction, edge,
                                            ccp, cp, d))
    {
        cl_out = cp.add(cl_in.sub(ccp));
        ccp_out = ccp;
        d_out = d;
        cp_out = cp;
        return true;
    }
    d_out = INFINITE;
    return false;
}

bool 
CylindricalCutter::intersect_circle_triangle(const Point& cl_in, const Point& direction, const Triangle& triangle,
                                             Point& cl_out, double& d_out, Point& cp_out) const
{
    Point cl;
    Point ccp;
    Point cp;
    double d;
    if (intersect_circle_plane(cl_in, direction, triangle, cl, ccp, d, cp))
    {
        if (Intersection::point_inside_triangle(cp, triangle))
        {
            cl_out = cl;
            d_out = d;
            cp_out = cp;
            return true;
        }
    }
    d_out = INFINITE;
    return false;
}

bool 
CylindricalCutter::intersect_circle_vertex(const Point& cl_in, const Point& direction, const Point& point,
                                           Point& cl_out, double& d_out, Point& cp_out) const
{
    Point cl;
    Point ccp;
    Point cp;
    double d;
    if (intersect_circle_point(cl_in, direction, point, 
                               cl, ccp, d, cp))
    {
        cl_out = cl;
        d_out = d;
        cp_out = cp;
        return true;
    }
    d_out = INFINITE;
    return false;
}

bool 
CylindricalCutter::intersect_circle_edge(const Point& cl_in, const Point& direction, const Line& edge,
                                         Point& cl_out, double& d_out, Point& cp_out) const
{
    Point cl;
    Point ccp;
    Point cp;
    double d;
    if (intersect_circle_line(cl_in, direction, edge, 
                              cl, ccp, d, cp))
    {
        // check if the contact point is between the endpoints
        double m = cp.sub(edge.p1()).dot(edge.dir());
        if (m > -epsilon && m < edge.len() + epsilon) 
        {
            cl_out = cl;
            d_out = d;
            cp_out = cp;
            return true;
        }
    }
    d_out = INFINITE;
    return false;
}

bool 
CylindricalCutter::intersect(const Point& cl_in, const Point& direction, const Triangle& triangle,
                             Point& cl_out, double& d_out, Point& cp_out) const
{
    Point center = cl_in.add(Point(0,0,m_radius));
    double d = INFINITE;
    Point cl;
    Point cp;

    {
        Point cl_t;
        Point cp_t;
        double d_t;
        if (intersect_circle_triangle(center, direction, triangle, cl_t, d_t, cp_t))
        {
            d = d_t;
            cl = cl_t;
            cp = cp_t;
            if (d != INFINITE && isZero(direction.x()) && isZero(direction.y())) {
                cl_out = cl;
                d_out = d;
                cp_out = cp;
                return true;
            }
        }
    }
    {
        Point cl_e1;
        double d_e1;
        Point cp_e1;
        if (intersect_circle_edge(center, direction, triangle.e1(), cl_e1, d_e1, cp_e1)) {
            if (d_e1 < d) {
                d = d_e1;
                cl = cl_e1;
                cp = cp_e1;
            }
        }
    }
    {
        Point cl_e2;
        double d_e2;
        Point cp_e2;
        if (intersect_circle_edge(center, direction, triangle.e2(), cl_e2, d_e2, cp_e2)) {
            if (d_e2 < d) {
                d = d_e2;
                cl = cl_e2;
                cp = cp_e2;
            }
        }
    }
    {
        Point cl_e3;
        double d_e3;
        Point cp_e3;
        if (intersect_circle_edge(center, direction, triangle.e3(), cl_e3, d_e3, cp_e3)) {
            if (d_e3 < d) {
                d = d_e3;
                cl = cl_e3;
                cp = cp_e3;
            }
        }
    }
    if (d != INFINITE && isZero(direction.x()) && isZero(direction.y())) {
        cl_out = cl;
        d_out = d;
        cp_out = cp;
        return true;
    }

    {
        Point cl_p1;
        double d_p1;
        Point cp_p1;
        if (intersect_circle_vertex(center, direction, triangle.p1(), cl_p1, d_p1, cp_p1)) {
            if (d_p1 < d) {
                d = d_p1;
                cl = cl_p1;
                cp = cp_p1;
            }
        }
    }
    {
        Point cl_p2;
        double d_p2;
        Point cp_p2;
        if (intersect_circle_vertex(center, direction, triangle.p2(), cl_p2, d_p2, cp_p2)) {
            if (d_p2 < d) {
                d = d_p2;
                cl = cl_p2;
                cp = cp_p2;
            }
        }
    }
    {
        Point cl_p3;
        double d_p3;
        Point cp_p3;
        if (intersect_circle_vertex(center, direction, triangle.p3(), cl_p3, d_p3, cp_p3)) {
            if (d_p3 < d) {
                d = d_p3;
                cl = cl_p3;
                cp = cp_p3;
            }
        }
    }
    if (d != INFINITE && isZero(direction.x()) && isZero(direction.y())) {
        cl_out = cl;
        d_out = d;
        cp_out = cp;
        return true;
    }
    if (!isZero(direction.x()) || !isZero(direction.y())) {
        {
            Point cl_p1;
            double d_p1;
            Point cp_p1;
            if (intersect_cylinder_vertex(center, direction, triangle.p1(), cl_p1, d_p1, cp_p1)) {
                if (d_p1 < d) {
                    d = d_p1;
                    cl = cl_p1;
                    cp = cp_p1;
                }
            }
        }
        {
            Point cl_p2;
            double d_p2;
            Point cp_p2;
            if (intersect_cylinder_vertex(center, direction, triangle.p2(), cl_p2, d_p2, cp_p2)) {
                if (d_p2 < d) {
                    d = d_p2;
                    cl = cl_p2;
                    cp = cp_p2;
                }
            }
        }
        {
            Point cl_p3;
            double d_p3;
            Point cp_p3;
            if (intersect_cylinder_vertex(center, direction, triangle.p3(), cl_p3, d_p3, cp_p3)) {
                if (d_p3 < d) {
                    d = d_p3;
                    cl = cl_p3;
                    cp = cp_p3;
                }
            }
        }
        {
            Point cl_e1;
            double d_e1;
            Point cp_e1;
            if (intersect_cylinder_edge(center, direction, triangle.e1(), cl_e1, d_e1, cp_e1)) {
                if (d_e1 < d) {
                    d = d_e1;
                    cl = cl_e1;
                    cp = cp_e1;
                }
            }
        }
        {
            Point cl_e2;
            double d_e2;
            Point cp_e2;
            if (intersect_cylinder_edge(center, direction, triangle.e2(), cl_e2, d_e2, cp_e2)) {
                if (d_e2 < d) {
                    d = d_e2;
                    cl = cl_e2;
                    cp = cp_e2;
                }
            }
        }
        {
            Point cl_e3;
            double d_e3;
            Point cp_e3;
            if (intersect_cylinder_edge(center, direction, triangle.e3(), cl_e3, d_e3, cp_e3)) {
                if (d_e3 < d) {
                    d = d_e3;
                    cl = cl_e3;
                    cp = cp_e3;
                }
            }
        }
    }
    if (d < INFINITE) {
        cl_out = cl;
        d_out = d;
        cp_out = cp;
        return true;
    }
    return false;
}
