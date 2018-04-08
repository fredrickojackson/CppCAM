/* -*- coding: utf-8 -*-

Copyright 2014 Lode Leroy

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
#include "SimpleCutter.h"
#include "PathProcessors.h"

bool
SimpleCutter::GenerateCutPath(const HeightField& heightfield, const Point& start, const Point& direction, const std::vector<double>& zlevels, 
                              std::vector<Path*>& paths, double angle)
{
    if (direction.z() != 0) return false;
    if (direction.x() != 0 && direction.y() != 0) return false;
    if (direction.x() != 0 && direction.y() == 0) {
        for(size_t i=0; i<zlevels.size(); i++) {
            Path* path = new Path();
            path->rot_x=angle;
            if (!GenerateCutPathLayer_y(heightfield, start, direction, zlevels[zlevels.size()-i-1], *path)) {
                delete path;
                return false;
            }
            paths.push_back(path);
        }
        return true;
    }
    if (direction.x() == 0 && direction.y() != 0) {
        for(size_t i=0; i<zlevels.size(); i++) {
            Path* path = new Path();
            if (!GenerateCutPathLayer_x(heightfield, start, direction, zlevels[zlevels.size()-i-1], *path)) {
                delete path;
                return false;
            }
            paths.push_back(path);
        }
        return true;
    }
}

bool
SimpleCutter::GenerateCutPathLayer_x(const HeightField& heightfield, const Point& start, const Point& direction, double z_layer, 
                                     Path& path)
{
    SimplePathProcessors pathProcessors;
    for(size_t j=0; j<heightfield.width(); j++) {
        path.m_runs.resize(path.m_runs.size()+1);
        Run& run = path.m_runs.back();
        double last_z = 0;
        for(size_t ii=0; ii<heightfield.height(); ii++) {
            size_t i = ii;
            if (m_zigzag) i = heightfield.height()-ii;

            double z = heightfield.point(j,i);
            if (z < z_layer) {
                z = z_layer;
            }
            run.m_points.push_back(Point(heightfield.x(j), heightfield.y(i), z));
        }
        pathProcessors.process(run.m_points);
    }
    return true;
}

bool
SimpleCutter::GenerateCutPathLayer_y(const HeightField& heightfield, const Point& start, const Point& direction, double z_layer, 
                                     Path& path)
{
    SimplePathProcessors pathProcessors;
    for(size_t i=0; i<heightfield.height(); i++) {
        path.m_runs.resize(path.m_runs.size()+1);
        Run& run = path.m_runs.back();
        double last_z = 0;
        for(size_t jj=0; jj<heightfield.width(); jj++) {
            size_t j = jj;
            if (m_zigzag) j = heightfield.width()-jj;

            double z = heightfield.point(j,i);
            if (z < z_layer) {
                z = z_layer;
            }
            run.m_points.push_back(Point(heightfield.x(j), heightfield.y(i), z));
        }
        pathProcessors.process(run.m_points);
    }
    return true;
}
