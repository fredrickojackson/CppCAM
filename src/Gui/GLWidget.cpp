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
#include <QtOpenGL>
#include <GL/glu.h>

#include <iostream>
#include <cmath>

#include "GLWidget.h"
#include "TestModel.h"
#include "STLImporter.h"

#include "Cutter.h"
#include "Path.h"
#include "Stock.h"
#include "HeightField.h"

extern Stock* stock;
extern Model* model;
extern Cutter* cutter;
extern Cutter* cutter2;
extern std::vector<Path*> paths;
extern HeightField* heightfield;

GLWidget::GLWidget(QWidget* parent) : QGLWidget(parent)
{
    mShowStock = true;
    mShowModel = true;
    mShowModelBoundingBox = true;
    mShowPath = true;
    mShowNormals = true;
    mShowCutter = true;
    mShowHeightField = true;

    resetView();
    setFocusPolicy(Qt::ClickFocus);
}

void
GLWidget::resetView()
{
    xRot=yRot=zRot=xDist=yDist=zDist=0;
    yDist = -1;
    zDist = -10.0;
    scale = 1.0;
    xRot = 110;
    yRot = 180;
    zRot = 160;
    scale = 0.2;
//    xDist = 0;
//    yDist = -1.0;
//    zDist = -8.0;
    updateGL();
}

void
GLWidget::setFrontView()
{
    xRot=yRot=zRot=xDist=yDist=zDist=0;
    yDist = -1;
    zDist = -10.0;
    scale = 1.0;
    xRot = 90;
    yRot = 180;
    zRot = 180;
    scale = 0.2;
//    xDist = 0;
//    yDist = -1.0;
//    zDist = -8.0;
    updateGL();
}

void
GLWidget::setLeftView()
{
    xRot=yRot=zRot=xDist=yDist=zDist=0;
    yDist = -1;
    zDist = -10.0;
    scale = 1.0;
    xRot = 90;
    yRot = 180;
    zRot = 0;
    scale = 0.2;
//    xDist = 0;
//    yDist = -1.0;
//    zDist = -8.0;
    updateGL();
}

void
GLWidget::setRightView()
{
    xRot=yRot=zRot=xDist=yDist=zDist=0;
    yDist = -1;
    zDist = -10.0;
    scale = 1.0;
    xRot = 90;
    yRot = 180;
    zRot = 90;
    scale = 0.2;
//    xDist = 0;
//    yDist = -1.0;
//    zDist = -8.0;
    updateGL();
}

void
GLWidget::setTopView()
{
    xRot=yRot=zRot=xDist=yDist=zDist=0;
    zDist = -10.0;
    scale = 1.0;
    xRot = 180;
    yRot = 180;
    zRot = 180;
    scale = 0.2;
//    xDist = 0;
//    yDist = -1.0;
//    zDist = -8.0;
    updateGL();
}

void
GLWidget::initializeGL()
{
    // Set up the rendering context, define display lists etc.:
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //glShadeModel(GL_SMOOTH);
    glShadeModel(GL_FLAT);


    glEnable(GL_LIGHT0);
    static GLfloat lightPosition[4] = { 10, 10, -10, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    static GLfloat lightAmbient[4] = { 0.5, 0.5, 0.5, 1.0 };
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    static GLfloat lightDiffuse[4] = { 0.1, 0.1, 0.1, 1.0 };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    static GLfloat lightSpecular[4] = { 0.1, 0.1, 0.1, 1.0 };
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    glMatrixMode(GL_MODELVIEW);
    //static GLfloat color[4] = { 212/255.0,158/255.,13/255.,1 };
    //glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.5);
    //glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void
GLWidget::resizeGL(int width, int height)
{
    // setup viewport, projection etc.:
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES_1
    //glOrthof(-0.5, +0.5, -0.5, +0.5, 1.0, 15.0);
#else
    //glOrtho(-0.5, +0.5, -0.5, +0.5, 1.0, 15.0);
#endif
    gluPerspective(30.0, float(width)/float(height), 1.0, 150.0);

    glMatrixMode(GL_MODELVIEW);
}

void
GLWidget::paintGL()
{
    // draw the scene:
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(xDist, yDist, zDist);
    glRotatef(xRot, 1.0, 0.0, 0.0);
    glRotatef(yRot, 0.0, 1.0, 0.0);
    glRotatef(zRot, 0.0, 0.0, 1.0);
    glScalef(scale, scale, scale);

    // axis
    glDisable(GL_LIGHTING);
    glColor3f(1,0,0);
    glBegin(GL_LINES);
    glVertex3f(0,0,0);
    glVertex3f(1,0,0);
    glEnd();
    glColor3f(0,1,0);
    glBegin(GL_LINES);
    glVertex3f(0,0,0);
    glVertex3f(0,1,0);
    glEnd();
    glColor3f(0,0,1);
    glBegin(GL_LINES);
    glVertex3f(0,0,0);
    glVertex3f(0,0,1);
    glEnd();

    glDisable(GL_CULL_FACE);

    if (model != NULL) {
        if (mShowModelBoundingBox) {
            glDisable(GL_LIGHTING);
            glColor3f(0.3,0.3,0.3);
            model->bb_OpenGL();
        }
        if (mShowModel) {
            static GLfloat color[4] = { 0,0,1,1 };
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
            glEnable(GL_LIGHTING);
            //glDisable(GL_LIGHTING);
            model->to_OpenGL();
        }
        if (mShowNormals) {
            glDisable(GL_LIGHTING);
            glColor3f(0.9,0.6,0.3);
            model->n_OpenGL();
        }
    }

    if (paths.size()>0 && mShowPath) {
        for(size_t i=0; i<paths.size(); i++) {
            glDisable(GL_LIGHTING);
            glColor3f(1,0,1);
            paths[i]->to_OpenGL(stock->min_z(),stock->max_z());
        }
    }

    if (cutter != NULL && mShowCutter) {
        static GLfloat color[4] = { .7,.7,.7,1 };
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);

        glEnable(GL_LIGHTING);
        cutter->to_OpenGL();
    }
    if (heightfield != NULL && mShowHeightField) {
        glDisable(GL_LIGHTING);
        glColor3f(1,1,1);
        heightfield->to_OpenGL();
    }

    if (stock != NULL && mShowStock) {
        stock->bb_OpenGL();
    }

}

void GLWidget::normalizeAngle(int& angle)
{
    angle = ((angle % 360) + 360) % 360;
}

void GLWidget::setXRotation(int angle)
{
    normalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setYRotation(int angle)
{
    normalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setZRotation(int angle)
{
    normalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    last_pos = event->pos();
    last_xRot = xRot;
    last_yRot = yRot;
    last_zRot = zRot;
    last_xDist=xDist;
    last_yDist=yDist;
    last_zDist=zDist;
}

void GLWidget::keyPressEvent(QKeyEvent* /*event*/)
{
    updateGL();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->modifiers() & Qt::ShiftModifier) {
        int dx = event->x() - last_pos.x();
        int dy = event->y() - last_pos.y();
        xDist = last_xDist + dx/(80+zDist);
        yDist = last_yDist - dy/(120+zDist);
        event->accept();
        updateGL();
        return;
    }

#if 0
    int dx = event->x() - last_pos.x();
    int dy = event->y() - last_pos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot - dy);
        setYRotation(yRot + dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot + dy);
        setZRotation(zRot + dx);
    }
    //std::cout << "x=" << xRot << ", y=" << yRot << ", z=" << zRot << std::endl;
    lastPos = event->pos();
#else
    int x = event->x();
    int y = event->y();
    if (event->buttons() & Qt::LeftButton || event->buttons() & Qt::RightButton) {
        double a3 = acos(float(last_pos.x())/width()-0.5);
        double a4 = acos(float(x)/width()-0.5);
        setZRotation(last_zRot - (a4-a3)*180/M_PI*8);
    }
    if (event->buttons() & Qt::RightButton) {
        double a3 = acos(float(last_pos.y())/height()-0.5);
        double a4 = acos(float(y)/height()-0.5);
        if (x > width()/2.0) {
            setYRotation(last_yRot + (a4-a3)*180/M_PI*8);
        } else {
            setYRotation(last_yRot - (a4-a3)*180/M_PI*8);
        }
    }
    if (event->buttons() & Qt::LeftButton) {
        double a3 = acos(float(last_pos.y())/height()-0.5);
        double a4 = acos(float(y)/height()-0.5);
        setXRotation(last_xRot - (a4-a3)*180/M_PI*8);
    }
#endif
    event->accept();
    updateGL();
    return;
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    if (numDegrees != 0) {
        double numSteps = numDegrees / 15.0;
        if (numSteps > 0) {
            scale /= 1.10;
        } else if (numSteps < 0) {
            scale *= 1.10;
        }
    }
    event->accept();
    updateGL();
}
