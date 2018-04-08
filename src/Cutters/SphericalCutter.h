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
#ifndef __SPHERICALCUTTER_H
#define __SPHERICALCUTTER_H

#include "BaseCutter.h"

class SphericalCutter : public BaseCutter
{
public:
    SphericalCutter(double radius, const Point& location);
    ~SphericalCutter();
    void to_OpenGL();

    bool intersect_sphere_plane(const Point& cl_in, const Point& direction, const Triangle& triangle,
                                Point& cl_out, Point& ccp_out, double& d_out, Point& cp_out) const;

    bool intersect_sphere_point(const Point& cl_in, const Point& direction, const Point& point,
                                Point& cl_out, Point& ccp_out, double& d_out, Point& cp_out) const;

    bool intersect_sphere_line(const Point& cl_in, const Point& direction, const Line& edge,
                               Point& cl_out, Point& ccp_out, double& d_out, Point& cp_out) const;

    bool intersect_sphere_triangle(const Point& cl_in, const Point& direction, const Triangle& triangle,
                                   Point& cl_out, double& d_out, Point& cp_out) const;

    bool intersect_sphere_vertex(const Point& cl_in, const Point& direction, const Point& point,
                                 Point& cl_out, double& d_out, Point& cp_out) const;

    bool intersect_sphere_edge(const Point& cl_in, const Point& direction, const Line& edge,
                               Point& cl_out, double& d_out, Point& cp_out) const;

    bool intersect(const Point& cl_in, const Point& direction, const Triangle& triangle,
                   Point& cl_out, double& d_out, Point& cp_out) const;

    virtual bool drop(const Point& cl_in, const Model& model, Point& cl_out) const;

private:
    GLUquadric* m_sphere;
};

#endif
