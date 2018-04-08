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
#include <cstdio>
#include <iostream>
#include "Geometry.h"

volatile double x=1,y=1,z=1;

std::ostream& operator<<( std::ostream& os, const Point& p ) { os << "<" << p.x << "," << p.y << "," << p.z << ">"; return os; }

int main(int argc, char* argv[])
{
    Point p0(1,0,0);
    Point p1(0,.1,0);

    std::cout << "p0=" << p0 << std::endl;
    std::cout << "p1=" << p1 << std::endl;
    std::cout << "p0 x p1=" << p0.cross(p1) << std::endl;
    std::cout << "|p0 x p1|^2=" << p0.cross(p1).norm_squared() << std::endl;
    std::cout << "|p0 x p1|=" << p0.cross(p1).norm() << std::endl;
    std::cout << "p1/|p1|=" << p1.normalized() << std::endl;
    Point p(p1);
    Point::normalize(p);
    std::cout << "p1/|p1|=" << p << std::endl;
}
