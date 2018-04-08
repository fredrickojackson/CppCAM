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
#ifndef __HEIGHTFIELD_H
#define __HEIGHTFIELD_H

#include <vector>
#include <cstddef>

struct HeightField
{
    HeightField(double x0, double x1, double y0, double y1, double z0, double /*z1*/, size_t width, size_t height) {
        m_x.resize(width);
        for(size_t i=0; i<width; i++) {
            m_x[i] = x0 + (x1-x0)*((double)(i))/(width-1);
        }
        m_y.resize(height);
        for(size_t i=0; i<height; i++) {
            m_y[i] = y0 + (y1-y0)*((double)(i))/(height-1);
        }
        m_points.resize(height);
        for(size_t i=0; i<height; i++) {
            m_points[i].resize(width);
        }

        for(size_t i=0; i<height; i++) {
            for(size_t j=0; j<width; j++) {
                point(j,i)=z0;
            }
        }
    }
    double x0() const { return m_x.front(); }
    double x1() const { return m_x.back(); }
    double y0() const { return m_y.front(); }
    double y1() const { return m_y.back(); }
    size_t width() const { return m_x.size(); }
    size_t height() const { return m_y.size(); }

    double x(size_t i) const { return m_x[i]; }
    double y(size_t i) const { return m_y[i]; }
    double point(int i, int j) const { return m_points[j][i]; }
    double& point(int i, int j) { return m_points[j][i]; }
    std::vector<double> m_x;
    std::vector<double> m_y;
    std::vector<std::vector<double> > m_points;
    void to_OpenGL();
};

#endif
