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
#include "HeightField.h"

#include <QtOpenGL>

#include <stdio.h>

void 
HeightField::to_OpenGL()
{
    glBegin(GL_LINES);
    for(size_t i=0; i<height(); i++) {
        for(size_t j=0; j<width(); j++) {
            glVertex3f(m_x[j], m_y[i], 0);
            glVertex3f(m_x[j], m_y[i], point(j,i));
        }
    }
    glEnd();
}

