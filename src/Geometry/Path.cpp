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
#include "Path.h"

#include <QtOpenGL>

void Path::to_OpenGL( float minz, float maxz)
{
    for(std::vector<Run>::iterator r = m_runs.begin(); r != m_runs.end(); ++r) {
        glBegin(GL_LINE_STRIP);
        //glBegin(GL_POINTS);
        for(std::vector<Point>::iterator p = r->m_points.begin(); p != r->m_points.end(); ++p) {
//            GLfloat color=fabs(p->z()-minz)/fabs(maxz-minz);
//            if(p->m_rad>0)
                glColor3f(1,0.5,1);
//            else
//                glColor3f(1,color,1);

            glVertex3f(p->x(),p->y(),p->z());
        }
        glEnd();

//        glBegin(GL_LINES);
//        for(std::vector<Point>::iterator p = r->m_points.begin(); p != r->m_points.end(); ++p) {

//            glVertex3f(p->x()-0.03,p->y(),p->z());
//            glVertex3f(p->x()+0.03,p->y(),p->z());
//            glVertex3f(p->x(),p->y()-0.03,p->z());
//            glVertex3f(p->x(),p->y()+0.03,p->z());
//            glVertex3f(p->x(),p->y(),p->z()-0.03);
//            glVertex3f(p->x(),p->y(),p->z()+0.03);
//        }
//        glEnd();
    }
}
