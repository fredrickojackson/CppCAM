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
#include "PathProcessors.h"

#include "Path.h"

#include <QSettings>
#include <iostream>


const double epsilon = 1e-5;

bool isNear(double x1, double x2)
{
    bool rvl = ((x2-x1)<epsilon) && ((x1-x2)<epsilon);
    //std::cout << x1 << " == " << x2 << " : " << (rvl ? "true":"false") << std::endl;
    return rvl;
}

// dy2/dx2 == dy1/dx1
bool isNear(double dy1, double dx1, double dy2, double dx2)
{
    bool rvl = isNear(dy2*dx1, dy1*dx2);
    //std::cout << dy1 << "/" << dx1  << "(" << (dy1/dx1) << ")" << " near " << dy2 << "/" << dx2 << "(" << (dy2/dx2) << ")" << " : " << (rvl ? "true":"false") << std::endl;
    return rvl;
}


template<typename T, typename V>
void insert_before(T& container, size_t index, const V& value)
{
    container.insert(container.begin()+index, value);
}

template<typename T, typename V>
void insert_after(T& container, size_t index, const V& value)
{
    container.insert(container.begin()+index+1, value);
}

template<typename T>
void erase_at(T& container, size_t index)
{
    container.erase(container.begin()+index);
}

StraightSegmentDetector::StraightSegmentDetector()
{
}

void 
StraightSegmentDetector::process(std::vector<Point>& points)
{
    if (points.size() <= 2) return;

    size_t last = points.size()-1;

    for(size_t pt = 1; pt < last; ) {
        if (points[0].x() == points[last].x()) {
            if (points[pt].z() == points[pt-1].z() && points[pt].z() == points[pt+1].z()) {
                erase_at(points, pt);
                last--;
                continue;
            }
            if (isNear(points[pt+1].z() - points[pt].z(), points[pt+1].y() - points[pt].y(), points[pt].z() - points[pt-1].z(), points[pt].y() - points[pt-1].y())) {
                erase_at(points, pt);
                last--;
                continue;
            }
        }

        if (points[0].y() == points[last].y()) {
            if (points[pt].z() == points[pt-1].z() && points[pt+1].z() == points[pt].z()) {
                erase_at(points, pt);
                last--;
                continue;
            }
            if (isNear(points[pt+1].z() - points[pt].z(), points[pt+1].x() - points[pt].x(), points[pt].z() - points[pt-1].z(), points[pt].x() - points[pt-1].x())) {
                erase_at(points, pt);
                last--;
                continue;
            }
        }
        pt++;
    }
}

StepUpDetector::StepUpDetector()
{
    max_slope = 10;
    policy = flat;
}

void 
StepUpDetector::process(std::vector<Point>& points)
{
    if (points.size() <= 2) return;

    size_t last = points.size()-1;

    if (points[0].y() == points[last].y()) {
        for(size_t pt = 1; pt < last; ) {
            if (points[pt].z() - points[pt-1].z() > fabs(points[pt].x() - points[pt-1].x()) * max_slope) {
                if (policy == flat) {
                    insert_before(points, pt, Point(points[pt-1].x(), points[pt-1].y(), points[pt].z()));
                }
                if (policy == extend) {
                    double dz = (points[pt+1].z()-points[pt].z())/(points[pt+1].x()-points[pt].x())*(points[pt-1].x()-points[pt].x());
                    insert_before(points, pt, Point(points[pt-1].x(), points[pt-1].y(), points[pt].z()-dz));
                }
                if (policy == extend_down) {
                    double dz = (points[pt+1].z()-points[pt].z())/(points[pt+1].x()-points[pt].x())*(points[pt-1].x()-points[pt].x());
                    if(dz>0) dz=0;
                    insert_before(points, pt, Point(points[pt-1].x(), points[pt-1].y(), points[pt].z()+dz));
                }
                pt+=2;
                last++;
                continue;
            }
            pt++;
        }
    }

    if (points[0].x() == points[last].x()) {
        for(size_t pt = 1; pt < last; ) {
            if (points[pt].z() - points[pt-1].z() > fabs(points[pt].y() - points[pt-1].y()) * max_slope) {
                if (policy == flat) {
                    insert_before(points, pt, Point(points[pt-1].x(), points[pt-1].y(), points[pt].z()));
                }
                if (policy == extend) {
                    double dz = (points[pt+1].z()-points[pt].z())/(points[pt+1].y()-points[pt].y())*(points[pt-1].y()-points[pt].y());
                    insert_before(points, pt, Point(points[pt-1].x(), points[pt-1].y(), points[pt].z()-dz));
                }
                if (policy == extend_down) {
                    double dz = (points[pt+1].z()-points[pt].z())/(points[pt+1].y()-points[pt].y())*(points[pt-1].y()-points[pt].y());
                    if(dz>0) dz=0;
                    insert_before(points, pt, Point(points[pt-1].x(), points[pt-1].y(), points[pt].z()+dz));
                }
                pt+=2;
                last++;
                continue;
            }
            pt++;
        }
    }
}

StepDownDetector::StepDownDetector()
{
    max_slope = 10;
    policy = flat;
}

void 
StepDownDetector::process(std::vector<Point>& points)
{
    if (points.size() <= 2) return;

    size_t last = points.size()-1;

    if (points[0].y() == points[last].y()) {
        for(size_t pt = 1; pt < last; ) {
            if (points[pt+1].z() - points[pt].z() < -fabs(points[pt+1].x() - points[pt].x()) * max_slope) {
                if (policy == flat) {
                    insert_after(points, pt, Point(points[pt+1].x(), points[pt+1].y(), points[pt].z()));
                }
                if (policy == extend) {
                    double dz = (points[pt].z()-points[pt-1].z())/(points[pt].x()-points[pt-1].x())*(points[pt+1].x()-points[pt].x());
                    insert_after(points, pt, Point(points[pt+1].x(), points[pt+1].y(), points[pt].z()+dz));
                }
                if (policy == extend_down) {
                    double dz = (points[pt].z()-points[pt-1].z())/(points[pt].x()-points[pt-1].x())*(points[pt+1].x()-points[pt].x());
                    if (dz>0) dz=0;
                    insert_after(points, pt, Point(points[pt+1].x(), points[pt+1].y(), points[pt].z()+dz));
                }
                pt+=2;
                last++;
                continue;
            }
            pt++;
        }
    }

    if (points[0].x() == points[last].x()) {
        for(size_t pt = 1; pt < last; ) {
            if (points[pt+1].z() - points[pt].z() < -fabs(points[pt+1].y() - points[pt].y()) * max_slope) {
                if (policy == flat) {
                    insert_after(points, pt, Point(points[pt+1].x(), points[pt+1].y(), points[pt].z()));
                }
                if (policy == extend) {
                    double dz = (points[pt].z()-points[pt-1].z())/(points[pt].y()-points[pt-1].y())*(points[pt+1].y()-points[pt].y());
                    insert_after(points, pt, Point(points[pt+1].x(), points[pt+1].y(), points[pt].z()+dz));
                }
                if (policy == extend_down) {
                    double dz = (points[pt].z()-points[pt-1].z())/(points[pt].y()-points[pt-1].y())*(points[pt+1].y()-points[pt].y());
                    if (dz>0) dz=0;
                    insert_after(points, pt, Point(points[pt+1].x(), points[pt+1].y(), points[pt].z()+dz));
                }
                pt+=2;
                last++;
                continue;
            }
            pt++;
        }
    }

}

SimplePathProcessors::SimplePathProcessors()
{
#if 0
    extern QSettings* settings;
    double max_slope = settings->value("max_slope").toDouble();
    m_StepUpDetector.max_slope = max_slope;
    m_StepDownDetector.max_slope = max_slope;
    QString slope_policy = settings->value("slope_policy").toString();
    if (max_slope_policy == "extend") {
        m_StepUpDetector.policy = StepUpDetector::extend;
        m_StepDownDetector.policy = StepDownDetector::extend;
    } else if (max_slope_policy == "extend_down") {
        m_StepUpDetector.policy = StepUpDetector::extend_down;
        m_StepDownDetector.policy = StepDownDetector::extend_down;
    } else {
        m_StepUpDetector.policy = StepUpDetector::flat;
        m_StepDownDetector.policy = StepDownDetector::flat;
    }
#endif
}

void
SimplePathProcessors::process(std::vector<Point>& run)
{
    m_StepUpDetector.process(run);
    m_StepDownDetector.process(run);
    m_StraightSegmentDetector.process(run);
}
