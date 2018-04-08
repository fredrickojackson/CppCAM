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

#ifndef __TRIANGLE_H
#define __TRIANGLE_H

#include "Point.h"
#include "Edge.h"

#include <vector>
#include <cmath>

#include <iostream>

struct Triangle
{
    Triangle(const Point& p1, const Point& p2, const Point& p3, const Point& n)
    {
        std::vector<Point>* points = new std::vector<Point>();
        points->push_back(p1);
        points->push_back(p2);
        points->push_back(p3);
        std::vector<Edge>* edges = new std::vector<Edge>();
        edges->push_back(Edge(*points,0,1));
        edges->push_back(Edge(*points,1,2));
        edges->push_back(Edge(*points,2,0));
        std::vector<Point>* normals = new std::vector<Point>();
        normals->push_back(n);
        m_points = points;
        m_edges = edges;
        m_normals = normals;
        m_idx_p1=0;
        m_idx_p2=1;
        m_idx_p3=2;
        m_idx_e1=0;
        m_idx_e2=1;
        m_idx_e3=2;
        m_idx_n=0;
        calculate_boundingbox();
    }
    Triangle(std::vector<Point>& points, std::vector<Point>& normals, std::vector<Edge>& edges, size_t idx_p1, size_t idx_p2, size_t idx_p3, size_t idx_e1, size_t idx_e2, size_t idx_e3, size_t idx_n=INFINITE) :
        m_points(&points),m_normals(&normals),m_edges(&edges),
        m_idx_p1(idx_p1),m_idx_p2(idx_p2),m_idx_p3(idx_p3),m_idx_e1(idx_e1),m_idx_e2(idx_e2),m_idx_e3(idx_e3),m_idx_n(idx_n) {
        if (m_idx_n >= normals.size()) {
            m_idx_n = normals.size();
            Point n = Point::cross_normalized(Point::sub(p3(),p1()),Point::sub(p2(),p1()));
            if (n.z() < 0) n = n.mul(-1);
            normals.push_back(n);
        }
        calculate_boundingbox();
    }
    void calculate_boundingbox()
    {
        m_min_x = std::min(p1().x(), std::min(p2().x(), p3().x()));
        m_min_y = std::min(p1().y(), std::min(p2().y(), p3().y()));
        m_min_z = std::min(p1().z(), std::min(p2().z(), p3().z()));
        m_max_x = std::max(p1().x(), std::max(p2().x(), p3().x()));
        m_max_y = std::max(p1().y(), std::max(p2().y(), p3().y()));
        m_max_z = std::max(p1().z(), std::max(p2().z(), p3().z()));
        m_center = p1().add(p2()).add(p3()).div(3);
        m_radiussq = center().sub(p1()).normsq();
        m_radius = sqrt(m_radiussq);
    }
    const std::vector<Point>* m_points;
    const std::vector<Point>* m_normals;
    const std::vector<Edge>* m_edges;
    size_t m_idx_p1;
    size_t m_idx_p2;
    size_t m_idx_p3;
    size_t m_idx_e1;
    size_t m_idx_e2;
    size_t m_idx_e3;
    size_t m_idx_n;
    double m_min_x;
    double m_max_x;
    double m_min_y;
    double m_max_y;
    double m_min_z;
    double m_max_z;
    Point m_center;
    double m_radiussq;
    double m_radius;
    const Point& p1() const { return (*m_points)[m_idx_p1]; }
    const Point& p2() const { return (*m_points)[m_idx_p2]; }
    const Point& p3() const { return (*m_points)[m_idx_p3]; }
    const Edge& e1() const { return (*m_edges)[m_idx_e1]; }
    const Edge& e2() const { return (*m_edges)[m_idx_e2]; }
    const Edge& e3() const { return (*m_edges)[m_idx_e3]; }
    const Point& n() const { return (*m_normals)[m_idx_n]; }
    const Point& center() const { return m_center; }
    double radiussq() const { return m_radiussq; }
    double radius() const { return m_radius; }
    double min_x() const { return m_min_x; }
    double max_x() const { return m_max_x; }
    double min_y() const { return m_min_y; }
    double max_y() const { return m_max_y; }
    double min_z() const { return m_min_z; }
    double max_z() const { return m_max_z; }
};

#endif
