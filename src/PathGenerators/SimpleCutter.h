/* -  *- coding: utf-8 -*-

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
#ifndef __SIMPLECUTTER_H
#define __SIMPLECUTTER_H

#include "Path.h"
#include "Stock.h"
#include "HeightField.h"
#include <vector>

class SimpleCutter {
public:
    SimpleCutter():m_zigzag(false) { }
    bool GenerateCutPath(const HeightField& heightfield, const Point& start, const Point& direction, const std::vector<double>& zlevels, std::vector<Path*>& paths, double angle);
    bool GenerateCutPathx(const HeightField& heightfield, const Point& start, const Point& direction, double Zlevel, std::vector<Path*>& paths, double angle);
    bool GenerateCutPath_radial(const HeightField& heightfield, const Point& start, const Point& direction, double Zlevel, Path* path, double angle);
    bool m_zigzag=false;

    double m_radius;
    QString m_type;
    double m_compmargin;
//    Stock* m_stock;
    double sminx;
    double smaxx;
    double sminy;
    double smaxy;
    double sminz;
    double smaxz;
    int m_useLine;
    int m_smooth;
    bool m_noretrace=false;
    double m_radialcut=0.0;
    double m_radialcutdepth=0.0;

private:
    bool GenerateCutPathLayer_x(const HeightField& heightfield, const Point& start, const Point& direction, double z_layer, Path& path);
    bool GenerateCutPathLayer_y(const HeightField& heightfield, const Point& start, const Point& direction, double z_layer, Path& path);
    bool GenerateCutPathLayer_rect(const HeightField& heightfield, const Point& start, const Point& direction, double z_layer, Path& path);
};

#endif
