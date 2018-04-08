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
#include <Boxed.h>

#include <QtOpenGL>

void 
Boxed::bb_OpenGL()
{
    glBegin(GL_LINES);
    glVertex3f(min_x(),min_y(),min_z());
    glVertex3f(max_x(),min_y(),min_z());
    glVertex3f(min_x(),max_y(),min_z());
    glVertex3f(max_x(),max_y(),min_z());
    glVertex3f(min_x(),min_y(),max_z());
    glVertex3f(max_x(),min_y(),max_z());
    glVertex3f(min_x(),max_y(),max_z());
    glVertex3f(max_x(),max_y(),max_z());

    glVertex3f(min_x(),min_y(),min_z());
    glVertex3f(min_x(),max_y(),min_z());
    glVertex3f(max_x(),min_y(),min_z());
    glVertex3f(max_x(),max_y(),min_z());
    glVertex3f(min_x(),min_y(),max_z());
    glVertex3f(min_x(),max_y(),max_z());
    glVertex3f(max_x(),min_y(),max_z());
    glVertex3f(max_x(),max_y(),max_z());

    glVertex3f(min_x(),min_y(),min_z());
    glVertex3f(min_x(),min_y(),max_z());
    glVertex3f(max_x(),min_y(),min_z());
    glVertex3f(max_x(),min_y(),max_z());
    glVertex3f(min_x(),max_y(),min_z());
    glVertex3f(min_x(),max_y(),max_z());
    glVertex3f(max_x(),max_y(),min_z());
    glVertex3f(max_x(),max_y(),max_z());
    glEnd();
}

