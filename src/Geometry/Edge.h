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
#ifndef __EDGE_H
#define __EDGE_H
#include <stdlib.h>

#include "Point.h"
#include <vector>

struct Edge
{
    Edge(const std::vector<Point>& points, size_t idx_p1, size_t idx_p2) :
        m_points(&points),
        m_idx_p1(idx_p1),m_idx_p2(idx_p2) {}
    const std::vector<Point>* m_points;
    size_t m_idx_p1;
    size_t m_idx_p2;
    const Point& p1() const { return (*m_points)[m_idx_p1]; }
    const Point& p2() const { return (*m_points)[m_idx_p2]; }
    const Point dir() const { return p2().sub(p1()); }
};

#endif
