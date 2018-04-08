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
#include "PathProcessors.h"

int main()
{
    std::vector<Point> points;
    points.resize(0);
    points.push_back(Point(0,0,0));
    points.push_back(Point(1,0,0));
    points.push_back(Point(2,0,0));
    points.push_back(Point(3,0,3));
    points.push_back(Point(4,0,4));
    points.push_back(Point(5,0,5));
    points.push_back(Point(6,0,0));

    for(size_t i=0; i<points.size(); i++) {
        std::cout << " (" << points[i].z() << "," << points[i].z() << ")";
    }
    std::cout << std::endl;

    //StraightSegmentDetector::process(points);
    //StepUpDetector::process(points);
    StepDownDetector::process(points);

    for(size_t i=0; i<points.size(); i++) {
        std::cout << " (" << points[i].z() << "," << points[i].z() << ")";
    }
    std::cout << std::endl;
}
