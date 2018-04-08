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
#ifndef __PATHPROCESSORS_H
#define __PATHPROCESSORS_H

#include <vector>
class Path;
class Point;

class StraightSegmentDetector
{
public:
    StraightSegmentDetector();
    void process(std::vector<Point>& points);
};

class StepUpDetector
{
public:
    StepUpDetector();
    void process(std::vector<Point>& points);

    enum { flat, extend, extend_down } policy;
    double max_slope;
};

class StepDownDetector
{
public:
    StepDownDetector();
    void process(std::vector<Point>& points);
    enum { flat, extend, extend_down } policy;
    double max_slope;
};

class SimplePathProcessors
{
public:
    SimplePathProcessors();
    void process(std::vector<Point>& points);

    StraightSegmentDetector m_StraightSegmentDetector;
    StepUpDetector m_StepUpDetector;
    StepDownDetector m_StepDownDetector;
};

#endif
