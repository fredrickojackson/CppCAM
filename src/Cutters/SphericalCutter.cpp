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

#include "SphericalCutter.h"

#include "intersection.h"

#include <QtOpenGL>
#include <GL/glu.h>

SphericalCutter::SphericalCutter(double radius, const Point& location)
    :BaseCutter(radius, location),m_sphere(0) 
{
    m_cutterType="Sph";
}
 
SphericalCutter::~SphericalCutter()
{
    if (m_sphere) {
        gluDeleteQuadric(m_sphere);
    }
}

void 
SphericalCutter::to_OpenGL()
{
    glPushMatrix();
    glTranslatef(m_location.x(), m_location.y(), m_location.z() + m_radius);
    if (m_cylinder == NULL) {
        m_cylinder = gluNewQuadric();
    }
    gluCylinder(m_cylinder, m_radius, m_radius, m_height - m_radius, 50, 1);
    if (m_sphere == NULL) {
        m_sphere = gluNewQuadric();
    }
    gluSphere(m_sphere, m_radius, 50, 50);
    glPopMatrix();
}

bool 
SphericalCutter::intersect_sphere_plane(const Point& cl_in, const Point& direction, const Triangle& triangle,
                                        Point& cl_out, Point& ccp_out, double& d_out, Point& cp_out) const
{
    Point cl;
    Point ccp;
    Point cp;
    double d;
    if (Intersection::intersect_sphere_plane(cl_in, m_radius, direction, Plane(triangle.p1(), triangle.n()),
                                             ccp, cp, d))
    {
        if (ccp.z() < cl_in.z() + epsilon) {
            cl_out = cp.add(cl_in.sub(ccp));
            ccp_out = ccp;
            d_out = d;
            cp_out = cp;
            return true;
        }
    }
    d_out = INFINITE;
    return false;
}

bool 
SphericalCutter::intersect_sphere_point(const Point& cl_in, const Point& direction, const Point& point,
                                        Point& cl_out, Point& ccp_out, double& d_out, Point& cp_out) const
{
    Point cl;
    Point ccp;
    Point cp;
    double d;
    if (Intersection::intersect_sphere_point(cl_in, m_radius, m_radiussq, direction, point,
                                             ccp, cp, d))
    {
        if (ccp.z() < cl_in.z() + epsilon) {
            cl_out = cp.add(cl_in.sub(ccp));
            ccp_out = ccp;
            d_out = d;
            cp_out = cp;
            return true;
        }
    }
    d_out = INFINITE;
    return false;
}

bool 
SphericalCutter::intersect_sphere_line(const Point& cl_in, const Point& direction, const Line& edge,
                                       Point& cl_out, Point& ccp_out, double& d_out, Point& cp_out) const
{
    Point cl;
    Point ccp;
    Point cp;
    double d;
    if (Intersection::intersect_sphere_line(cl_in, m_radius, m_radiussq, direction, edge,
                                            ccp, cp, d))
    {
        if (ccp.z() < cl_in.z() + epsilon) {
            cl_out = cp.add(cl_in.sub(ccp));
            ccp_out = ccp;
            d_out = d;
            cp_out = cp;
            return true;
        }
    }
    d_out = INFINITE;
    return false;
}

bool 
SphericalCutter::intersect_sphere_triangle(const Point& cl_in, const Point& direction, const Triangle& triangle,
                                           Point& cl_out, double& d_out, Point& cp_out) const
{
    Point cl;
    Point ccp;
    Point cp;
    double d;
    if (intersect_sphere_plane(cl_in, direction, triangle, cl, ccp, d, cp))
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
SphericalCutter::intersect_sphere_vertex(const Point& cl_in, const Point& direction, const Point& point,
                                         Point& cl_out, double& d_out, Point& cp_out) const
{
    Point cl;
    Point ccp;
    Point cp;
    double d;
    if (intersect_sphere_point(cl_in, direction, point, 
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
SphericalCutter::intersect_sphere_edge(const Point& cl_in, const Point& direction, const Line& edge,
                                       Point& cl_out, double& d_out, Point& cp_out) const
{
    Point cl;
    Point ccp;
    Point cp;
    double d;
    if (intersect_sphere_line(cl_in, direction, edge, 
                              cl, ccp, d, cp))
    {
        // check if the contact point is between the endpoints
        double m = cp.sub(edge.p1()).dot(edge.dir());
        if (m > epsilon && m < edge.len() - epsilon) 
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
SphericalCutter::intersect(const Point& cl_in, const Point& direction, const Triangle& triangle,
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
        if (intersect_sphere_triangle(center, direction, triangle, cl_t, d_t, cp_t))
        {
            d = d_t;
            cl = cl_t.sub(Point(0,0,m_radius));
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
        if (intersect_sphere_edge(center, direction, triangle.e1(), cl_e1, d_e1, cp_e1)) {
            if (d_e1 < d) {
                d = d_e1;
                cl = cl_e1.sub(Point(0,0,m_radius));
                cp = cp_e1;
            }
        }
    }
    {
        Point cl_e2;
        double d_e2;
        Point cp_e2;
        if (intersect_sphere_edge(center, direction, triangle.e2(), cl_e2, d_e2, cp_e2)) {
            if (d_e2 < d) {
                d = d_e2;
                cl = cl_e2.sub(Point(0,0,m_radius));
                cp = cp_e2;
            }
        }
    }
    {
        Point cl_e3;
        double d_e3;
        Point cp_e3;
        if (intersect_sphere_edge(center, direction, triangle.e3(), cl_e3, d_e3, cp_e3)) {
            if (d_e3 < d) {
                d = d_e3;
                cl = cl_e3.sub(Point(0,0,m_radius));
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
        if (intersect_sphere_vertex(center, direction, triangle.p1(), cl_p1, d_p1, cp_p1)) {
            if (d_p1 < d) {
                d = d_p1;
                cl = cl_p1.sub(Point(0,0,m_radius));
                cp = cp_p1;
            }
        }
    }
    {
        Point cl_p2;
        double d_p2;
        Point cp_p2;
        if (intersect_sphere_vertex(center, direction, triangle.p2(), cl_p2, d_p2, cp_p2)) {
            if (d_p2 < d) {
                d = d_p2;
                cl = cl_p2.sub(Point(0,0,m_radius));
                cp = cp_p2;
            }
        }
    }
    {
        Point cl_p3;
        double d_p3;
        Point cp_p3;
        if (intersect_sphere_vertex(center, direction, triangle.p3(), cl_p3, d_p3, cp_p3)) {
            if (d_p3 < d) {
                d = d_p3;
                cl = cl_p3.sub(Point(0,0,m_radius));
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
                    cl = cl_p1.sub(Point(0,0,m_radius));
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
                    cl = cl_p2.sub(Point(0,0,m_radius));
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
                    cl = cl_p3.sub(Point(0,0,m_radius));
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
                    cl = cl_e1.sub(Point(0,0,m_radius));
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
                    cl = cl_e2.sub(Point(0,0,m_radius));
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
                    cl = cl_e3.sub(Point(0,0,m_radius));
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

bool 
SphericalCutter::drop(const Point& cl_in, const Model& model, Point& cl_out) const
{
    double z = - INFINITE;
    for(size_t i=0; i<model.m_triangles.size(); i++) {
        const Triangle& triangle = model.m_triangles[i];
        if (z - m_radius < triangle.max_z() + epsilon) 
        {
            Point cl;
            if (BaseCutter::drop(cl_in, triangle, cl)) {
                if (cl.z() > z) {
                    cl_out = cl;
                    z = cl.z();
                }
            }
        }
    }
    return z > -INFINITE;
}

