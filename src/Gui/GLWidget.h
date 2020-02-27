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
#include <QGLWidget>

class GLWidget : public QGLWidget {
    Q_OBJECT
public:
    GLWidget(QWidget* parent=NULL);
    double scale;
    void resizeGL(int w, int h);
private:
    void initializeGL();
    void paintGL();
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent* event);
    void normalizeAngle(int& angle);
public slots:
    void resetView();
    void setFrontView();
    void setTopView();
    void setLeftView();
    void setRightView();
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);

    void setShowStock(bool value) { mShowStock = value; }
    void setShowModel(bool value) { mShowModel = value; }
    void setShowModelBoundingBox(bool value) { mShowModelBoundingBox = value; }
    void setShowPath(bool value) { mShowPath = value; }
    void setShowNormals(bool value) { mShowNormals = value; }
    void setShowCutter(bool value) { mShowCutter = value; }
    void setShowHeightField(bool value) { mShowHeightField = value; }
signals:
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);
private:
    double xRot;
    double yRot;
    double zRot;
    double xDist;
    double yDist;
    double zDist;
    QPoint last_pos;
    double last_xRot;
    double last_yRot;
    double last_zRot;
    double last_xDist;
    double last_yDist;
    double last_zDist;

    bool mShowStock;
    bool mShowModel;
    bool mShowModelBoundingBox;
    bool mShowPath;
    bool mShowNormals;
    bool mShowCutter;
    bool mShowHeightField;
};
