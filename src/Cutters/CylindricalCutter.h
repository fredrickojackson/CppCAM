/* -*- coding: utf-8 -*-

Copyright 2014 Lode Leroy

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
#ifndef __CYLINDRICALCUTTER_H
#define __CYLINDRICALCUTTER_H

#include "BaseCutter.h"

class CylindricalCutter : public BaseCutter
{
public:
    CylindricalCutter(double radius, const Point& location);
    ~CylindricalCutter();
    void to_OpenGL();

    bool intersect_circle_plane(const Point& cl_in, const Point& direction, const Triangle& triangle,
                                Point& cl_out, Point& ccp_out, double& d_out, Point& cp_out) const;

    bool intersect_circle_point(const Point& cl_in, const Point& direction, const Point& point,
                                Point& cl_out, Point& ccp_out, double& d_out, Point& cp_out) const;

    bool intersect_circle_line(const Point& cl_in, const Point& direction, const Line& edge,
                               Point& cl_out, Point& ccp_out, double& d_out, Point& cp_out) const;

    bool intersect_circle_triangle(const Point& cl_in, const Point& direction, const Triangle& triangle,
                                   Point& cl_out, double& d_out, Point& cp_out) const;

   bool intersect_circle_vertex(const Point& cl_in, const Point& direction, const Point& point,
                                 Point& cl_out, double& d_out, Point& cp_out) const;

    bool intersect_circle_edge(const Point& cl_in, const Point& direction, const Line& edge,
                               Point& cl_out, double& d_out, Point& cp_out) const;

    bool intersect(const Point& cl_in, const Point& direction, const Triangle& triangle,
                   Point& cl_out, double& d_out, Point& cp_out) const;
private:
    GLUquadric* m_disk;
};

#endif
