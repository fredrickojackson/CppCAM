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
#ifndef __CUTTER_H
#define __CUTTER_H

#include "Geometry.h"
#include "Model.h"

class Cutter
{
public:
    Cutter(double radius, const Point& location);

    virtual void moveto(const Point& location) {
        m_location = location;
    }

    static Cutter* CreateSphericalCutter(double radius, const Point& location);
    static Cutter* CreateCylindricalCutter(double radius, const Point& location);

    double min_x(const Point& cl) const {
        return cl.x() - m_radius;
    }
    double max_x(const Point& cl) const {
        return cl.x() + m_radius;
    }
    double min_y(const Point& cl) const {
        return cl.y() - m_radius;
    }
    double max_y(const Point& cl) const {
        return cl.y() + m_radius;
    }
    virtual double min_z(const Point& cl) const {
        return cl.z();
    }
    virtual double max_z(const Point& cl) const {
        return cl.z() + m_height;
    }
    const Point& location() const { return m_location; }
    double radius() const { return m_radius; }

    virtual bool intersect(const Point& cl_in, const Point& direction, const Triangle& triangle, Point& cl_out, double& d_out, Point& cp_out) const = 0;

    virtual bool intersect(const Point& cl_in, const Point& direction, const Model& model, Point& cl_out, double& d_out, Point& cp_out) const;
    virtual bool drop(const Point& cl_in, const Triangle& triangle, Point& cl_out) const;
    virtual bool drop(const Point& cl_in, const Model& model, Point& cl_out) const;

    Point& drop(const Triangle& triangle);
    Point& drop(const Model& model);

    virtual bool push(const Point& cl_in, const Point& direction, const Triangle& triangle, Point& cl_out);
    virtual bool push(const Point& cl_in, const Point& direction, const Model& model, Point& cl_out);

    Point& push(const Point& direction, const Triangle& triangle);
    Point& push(const Point& direction, const Model& model);

    virtual void to_OpenGL()=0;

    bool m_isSpherical;

    double m_radius;

protected:
    Point m_location;
    double m_height;
    Point m_axis;
    double m_radiussq;
};

#endif
