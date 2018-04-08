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
#include <Stock.h>

#include <QtOpenGL>

void 
Stock::bb_OpenGL()
{
    glEnable(GL_CULL_FACE);

    glDisable(GL_LIGHTING);
    glColor3f(0.0,0.10,0.08);
    Boxed::bb_OpenGL();

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0,0.5,0.4,0.1);

    glBegin(GL_QUADS);

    glVertex3f(min_x(),min_y(),min_z());
    glVertex3f(min_x(),min_y(),max_z());
    glVertex3f(min_x(),max_y(),max_z());
    glVertex3f(min_x(),max_y(),min_z());

    glVertex3f(max_x(),min_y(),min_z());
    glVertex3f(max_x(),max_y(),min_z());
    glVertex3f(max_x(),max_y(),max_z());
    glVertex3f(max_x(),min_y(),max_z());

    glVertex3f(min_x(),min_y(),min_z());
    glVertex3f(min_x(),max_y(),min_z());
    glVertex3f(max_x(),max_y(),min_z());
    glVertex3f(max_x(),min_y(),min_z());

    glVertex3f(min_x(),min_y(),max_z());
    glVertex3f(max_x(),min_y(),max_z());
    glVertex3f(max_x(),max_y(),max_z());
    glVertex3f(min_x(),max_y(),max_z());

    glVertex3f(min_x(),min_y(),min_z());
    glVertex3f(min_x(),max_y(),min_z());
    glVertex3f(max_x(),max_y(),min_z());
    glVertex3f(max_x(),min_y(),min_z());

    glVertex3f(min_x(),min_y(),min_z());
    glVertex3f(max_x(),min_y(),min_z());
    glVertex3f(max_x(),min_y(),max_z());
    glVertex3f(min_x(),min_y(),max_z());

    glVertex3f(min_x(),max_y(),min_z());
    glVertex3f(min_x(),max_y(),max_z());
    glVertex3f(max_x(),max_y(),max_z());
    glVertex3f(max_x(),max_y(),min_z());

    glEnd();
}

