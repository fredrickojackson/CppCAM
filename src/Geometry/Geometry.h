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
#ifndef __GEOMETRY_H
#define __GEOMETRY_H

#include <cmath>

namespace {
    const double epsilon = 1e-5;
#ifndef INFINITE
    const double INFINITE = 1e+5;
#endif

    bool isNear(double a, double b) {
        return fabs(a - b) < epsilon;
    }

    bool isZero(double a) {
        return isNear(a, 0);
    }

    double sqr(double x) {
        return x * x;
    }
}

#include "Point.h"
#include "Edge.h"
#include "Triangle.h"

#endif
