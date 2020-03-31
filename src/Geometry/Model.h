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
#ifndef __MODEL_H
#define __MODEL_H

#include "Geometry.h"
#include <string>
#include "Boxed.h"

class Model : public Boxed
{
public:
    std::string m_name;

    void subdivide(double amount=0.2);
    void resize(double min_x, double scale_x, double min_y, double scale_y, double min_z, double scale_z);
    void rotate(double rot_x, double rot_y, double rot_z);
    void rotateip(double rot_x, double rot_y, double rot_z);

    void add_triangle(const Point* p1, const Point* p2, const Point* p3, const Point* n = NULL);
    void add_triangle(const Point& p1, const Point& p2, const Point& p3, const Point& n) {
        add_triangle(&p1, &p2, &p3, &n);
    }
    void add_triangle(const Point& p1, const Point& p2, const Point& p3) {
        add_triangle(&p1, &p2, &p3);
    }
    void to_OpenGL();
    void n_OpenGL();
    void calculate_boundingbox();
    std::vector<Point> m_points;
    std::vector<Point> m_normals;
    std::vector<Edge> m_edges;
    std::vector<Triangle> m_triangles;
};

#endif
