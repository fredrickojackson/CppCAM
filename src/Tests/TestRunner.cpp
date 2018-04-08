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
#include "Model.h"
#include "TestModel.h"

#include <iostream>

#include "Geometry.h"
#include "CylindricalCutter.h"

#include "intersection.h"

std::ostream& operator<<( std::ostream& os, const Point& p ) { os << "Point<" << p.x() << "," << p.y() << "," << p.z() << ">"; return os; }
std::ostream& operator<<( std::ostream& os, const Line& l ) { os << "Line<" << l.p1() << "," << l.p2() << ">"; return os; }
std::ostream& operator<<( std::ostream& os, const Triangle& t ) { os << "Triangle<" << t.p1() << "," << t.p2() << "," << t.p3() << "," << t.n() << ">"; return os; }
std::ostream& operator<<( std::ostream& os, const CylindricalCutter& c ) { os << "CylinderCutter<" << c.location() << "," << c.radius() << ">"; return os; }

void 
test_points()
{
    Point p0(1,0,0);
    Point p1(0,.1,0);

    std::cout << "p0=" << p0 << std::endl;
    std::cout << "p1=" << p1 << std::endl;
    std::cout << "p0 x p1=" << p0.cross(p1) << std::endl;
    std::cout << "|p0 x p1|^2=" << p0.cross(p1).normsq() << std::endl;
    std::cout << "|p0 x p1|=" << p0.cross(p1).norm() << std::endl;
    std::cout << "p1/|p1|=" << p1.normalized() << std::endl;
    Point p(p1);
    Point::normalize(p);
    std::cout << "p1/|p1|=" << p << std::endl;
    std::cout << "p0+p1=" << p0.add(p1) << std::endl;
    std::cout << "p1-p0=" << p1.sub(p0) << std::endl;
}

void
test_edge_closest_point()
{
    Point p0(0,0,1);
    Point p1(1,0,1);
    Point c(0.5, 0.5, 1);
    Point cp;
    if (Intersection::edge_closest_point(Line(p0,p1),c,cp)) {
        std::cout << cp << std::endl;
    } else {
        std::cout << "false" << std::endl;
    }
}

void
test_point_inside_triangle()
{
    Triangle t(Point(0,0,0),Point(1,0,0),Point(0,1,0),Point(0,0,1));
    Point p(.5,.5,0);
    if (Intersection::point_inside_triangle(p, t)) {
        std::cout << "true" << std::endl;
    } else {
        std::cout << "false" << std::endl;
    }
}

void
test_intersect_plane_point()
{
    Plane pl(Point(0,0,1),Point(0,0,1));
    Point p(.5,.5,0);
    Point d(1,1,1);
    Point cp;
    double l;
    if (Intersection::intersect_plane_point(pl, d, p, cp, l)) {
        std::cout << cp << std::endl;
    } else {
        std::cout << "false" << std::endl;
    }
}

void
test_intersect_circle_line()
{
    Point p1(0,0,0);
    Point p2(1,0,0.2);
    Point p3(0,1,0.5);

    Point cl = Point(0.5,0.5,1);
    Point axis = Point(0,0,1);
    Point dir = Point(0,0,1);
    double r = 1;
    Point cl_out;
    Point ccp_out;
    double d_out;
    Point cp_out;
#if 1
    std::cout << "cl=" << cl << std::endl;
    if (Intersection::intersect_circle_line(cl, axis, r, r*r, dir, Line(p1,p2), ccp_out, cl_out, d_out)) {
        std::cout << ccp_out << ", " << cl_out << ", " << d_out << std::endl;
    } else {
        std::cout << "false" << std::endl;
    }
#endif
#if 1
    std::cout << "cl=" << cl << std::endl;
    if (Intersection::intersect_circle_line(cl, axis, r, r*r, dir, Line(p2,p3), ccp_out, cl_out, d_out)) {
        std::cout << ccp_out << ", " << cl_out << ", " << d_out << std::endl;
    } else {
        std::cout << "false" << std::endl;
    }
#endif
#if 1
    std::cout << "cl=" << cl << ", dir=" << dir << std::endl;
    if (Intersection::intersect_circle_line(cl, axis, r, r*r, dir, Line(p3,p1), ccp_out, cl_out, d_out)) {
        std::cout << ccp_out << ", " << cl_out << ", " << d_out << std::endl;
    } else {
        std::cout << "false" << std::endl;
    }
#endif
}

void
test_intersect_cylinder_line()
{
    Point p1(0,0,0);
    Point p2(1,0,0);
    Point p3(0,1,0.5);

    Point cl = Point(0.5,-2,0);
    Point axis = Point(0,0,1);
    Point dir = Point(0,1,0);
    double r = 1;
    Point cl_out;
    Point ccp_out;
    double d_out;
    Point cp_out;
#if 0
    std::cout << "cl=" << cl << std::endl;
    if (Intersection::intersect_cylinder_line(cl, axis, r, r*r, dir, Line(p1,p2), ccp_out, cl_out, d_out)) {
        std::cout << ccp_out << ", " << cl_out << ", " << d_out << std::endl;
    } else {
        std::cout << "false" << std::endl;
    }
#endif
#if 1
    std::cout << "cl=" << cl << std::endl;
    if (Intersection::intersect_cylinder_line(cl, axis, r, r*r, dir, Line(p2,p3), ccp_out, cl_out, d_out)) {
        std::cout << ccp_out << ", " << cl_out << ", " << d_out << std::endl;
    } else {
        std::cout << "false" << std::endl;
    }
#endif
#if 0
    std::cout << "cl=" << cl << ", dir=" << dir << std::endl;
    if (Intersection::intersect_cylinder_line(cl, axis, r, r*r, dir, Line(p3,p1), ccp_out, cl_out, d_out)) {
        std::cout << ccp_out << ", " << cl_out << ", " << d_out << std::endl;
    } else {
        std::cout << "false" << std::endl;
    }
#endif
}

void
test1()
{
    CylindricalCutter c(1, Point(1,1,1));
    std::cout << c << std::endl;
    Triangle t(Point(0,0,0),Point(1,0,0),Point(0,1,0),Point(0,0,1));
    std::cout << t << std::endl;
    c.drop(t);
    std::cout << c.location() << std::endl;
    c.moveto(Point(0.5,0.5,1));
    c.drop(t);
    std::cout << c.location() << std::endl;
}


void
test2()
{
    CylindricalCutter c(1, Point(1,1,1));
    //std::cout << "c=" << c << std::endl;

    Point p1(0,0,0);
    Point p2(1,0,0);
    Point p3(0,1,0.5);
    Point n = Point::cross_normalized(Point::sub(p3,p1),Point::sub(p2,p1));
    if (n.z() < 0) n = n.mul(-1);

    Triangle t(p1,p2,p3,n);
    //c.drop(t);
    Point cl = Point(0.5,0.5,1);
    Point v = Point(0,0,1);
    Point cl_out;
    Point ccp_out;
    double d_out;
    Point cp_out;
#if 0
    std::cout << "l=" << t.e1() << std::endl;
    std::cout << "cl=" << cl << std::endl;
    if (c.intersect_circle_line(cl, v, Line(t.e1()), cl_out, ccp_out, d_out, cp_out)) {
        std::cout << cl_out << std::endl;
    } else {
        std::cout << "false" << std::endl;
    }
#endif
#if 1
    std::cout << "l=" << t.e2() << std::endl;
    std::cout << "cl=" << cl << std::endl;
    if (c.intersect_circle_line(cl, v, Line(t.e2()), cl_out, ccp_out, d_out, cp_out)) {
        std::cout << cl_out << std::endl;
    } else {
        std::cout << "false" << std::endl;
    }
#endif
}

int
run_tests()
{
    //test_points();
    //test_edge_closest_point();
    //test_point_inside_triangle();
    //test_intersect_plane_point();
    //test_intersect_circle_line();
    test_intersect_cylinder_line();
    return 0;
}
