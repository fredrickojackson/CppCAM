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

#include <QtOpenGL>

#include "TestModel.h"

TestModel::TestModel()
{
    double scale = 1.0;
    double dx = 0;
    double dy = 0;
    double dz = -1;

    m_points.push_back(Point((dx-2)*scale, (dy+1)*scale, (dz+4)*scale));
    m_points.push_back(Point((dx+2)*scale, (dy+1)*scale, (dz+4)*scale));
    m_points.push_back(Point((dx+0)*scale, (dy-2)*scale, (dz+4)*scale));
    m_points.push_back(Point((dx-5)*scale, (dy+2)*scale, (dz+2)*scale));
    m_points.push_back(Point((dx-1)*scale, (dy+3)*scale, (dz+2)*scale));
    m_points.push_back(Point((dx+5)*scale, (dy+2)*scale, (dz+2)*scale));
    m_points.push_back(Point((dx+4)*scale, (dy-1)*scale, (dz+2)*scale));
    m_points.push_back(Point((dx+2)*scale, (dy-4)*scale, (dz+2)*scale));
    m_points.push_back(Point((dx-2)*scale, (dy-4)*scale, (dz+2)*scale));
    m_points.push_back(Point((dx-3)*scale, (dy-2)*scale, (dz+2)*scale));
    
    calculate_boundingbox();

    m_edges.push_back(Edge(m_points, 0, 1));
    m_edges.push_back(Edge(m_points, 1, 2));
    m_edges.push_back(Edge(m_points, 2, 0));
    m_edges.push_back(Edge(m_points, 0, 3));
    m_edges.push_back(Edge(m_points, 3, 4));
    m_edges.push_back(Edge(m_points, 4, 0));
    m_edges.push_back(Edge(m_points, 4, 1));
    m_edges.push_back(Edge(m_points, 4, 5));
    m_edges.push_back(Edge(m_points, 5, 1));
    m_edges.push_back(Edge(m_points, 5, 6));
    m_edges.push_back(Edge(m_points, 6, 1));
    m_edges.push_back(Edge(m_points, 6, 2));
    m_edges.push_back(Edge(m_points, 6, 7));
    m_edges.push_back(Edge(m_points, 7, 2));
    m_edges.push_back(Edge(m_points, 7, 8));
    m_edges.push_back(Edge(m_points, 8, 2));
    m_edges.push_back(Edge(m_points, 8, 9));
    m_edges.push_back(Edge(m_points, 9, 2));
    m_edges.push_back(Edge(m_points, 9, 0));
    m_edges.push_back(Edge(m_points, 9, 3));

    m_triangles.push_back(Triangle(m_points, m_normals, m_edges, 0, 2, 1, 0, 1, 2));
    m_triangles.push_back(Triangle(m_points, m_normals, m_edges, 0, 4, 3, 3, 4, 5));
    m_triangles.push_back(Triangle(m_points, m_normals, m_edges, 0, 1, 4, 5, 6, 0));
    m_triangles.push_back(Triangle(m_points, m_normals, m_edges, 1, 5, 4, 6, 7, 8));
    m_triangles.push_back(Triangle(m_points, m_normals, m_edges, 1, 6, 5, 8, 9, 10));
    m_triangles.push_back(Triangle(m_points, m_normals, m_edges, 1, 2, 6, 10, 11, 1));
    m_triangles.push_back(Triangle(m_points, m_normals, m_edges, 2, 7, 6, 11, 12, 13));
    m_triangles.push_back(Triangle(m_points, m_normals, m_edges, 2, 8, 7, 13, 14, 15));
    m_triangles.push_back(Triangle(m_points, m_normals, m_edges, 2, 9, 8, 15, 16, 17));
    m_triangles.push_back(Triangle(m_points, m_normals, m_edges, 2, 0, 9, 17, 18, 2));
    m_triangles.push_back(Triangle(m_points, m_normals, m_edges, 0, 3, 9, 18, 19, 3));
}

