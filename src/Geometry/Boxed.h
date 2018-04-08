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
#ifndef __BOXED_H
#define __BOXED_H

class Boxed
{
public:
    double m_min_x;
    double m_max_x;
    double m_min_y;
    double m_max_y;
    double m_min_z;
    double m_max_z;

    double min_x() const { return m_min_x; }
    double max_x() const { return m_max_x; }
    double min_y() const { return m_min_y; }
    double max_y() const { return m_max_y; }
    double min_z() const { return m_min_z; }
    double max_z() const { return m_max_z; }
    double dim_x() const { return m_max_x - m_min_x; }
    double dim_y() const { return m_max_y - m_min_y; }
    double dim_z() const { return m_max_z - m_min_z; }

    virtual void bb_OpenGL();
};

#endif
