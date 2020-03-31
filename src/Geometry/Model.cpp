/* -*- coding: utf-8 -*-

Copyright 2008-2014 Lode Leroy

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

#include "Model.h"

#include <QtOpenGL>
#include <iostream>


namespace {
std::ostream& operator<<( std::ostream& os, const Point& p ) { os << "Point<" << p.x() << "," << p.y() << "," << p.z() << ">"; return os; }
std::ostream& operator<<( std::ostream& os, const Edge& e ) { os << "Edge<" << e.p1() << "," << e.p2() << ">"; return os; }
std::ostream& operator<<( std::ostream& os, const Triangle& t ) { os << "Triangle<" << t.p1() << "," << t.p2() << "," << t.p3() << "," << t.n() << ">"; return os; }
}

void 
Model::calculate_boundingbox()
{
    std::vector<Point>::const_iterator i=m_points.begin();

    if (i==m_points.end()) return;

    m_min_x = i->x();
    m_min_y = i->y();
    m_min_z = i->z();
    m_max_x = i->x();
    m_max_y = i->y();
    m_max_z = i->z();

    ++i;
    for(; i!=m_points.end(); ++i) {
        m_min_x = std::min(m_min_x, i->x());
        m_min_y = std::min(m_min_y, i->y());
        m_min_z = std::min(m_min_z, i->z());
        m_max_x = std::max(m_max_x, i->x());
        m_max_y = std::max(m_max_y, i->y());
        m_max_z = std::max(m_max_z, i->z());
    }
}

void
Model::add_triangle(const Point* p1, const Point* p2, const Point* p3, const Point* n)
{
    size_t idx_p1=INFINITE;
    size_t idx_p2=INFINITE;
    size_t idx_p3=INFINITE;
    size_t idx_n=INFINITE;
    if (n!=NULL&&n->x()==0&&n->y()==0&&n->z()==0) {
        n=NULL;
    }
    for(std::vector<Point>::iterator i=m_points.begin(); i!=m_points.end(); ++i)
    {
        if (i->x() == p1->x() && i->y() == p1->y() && i->z() == p1->z()) {
            idx_p1 = i - m_points.begin();
        }
        if (i->x() == p2->x() && i->y() == p2->y() && i->z() == p2->z()) {
            idx_p2 = i - m_points.begin();
        }
        if (i->x() == p3->x() && i->y() == p3->y() && i->z() == p3->z()) {
            idx_p3 = i - m_points.begin();
        }
    }
    if (idx_p1 == INFINITE) {
        idx_p1 = m_points.size();
        m_points.push_back(*p1);
    }
    if (idx_p2 == INFINITE) {
        idx_p2 = m_points.size();
        m_points.push_back(*p2);
    }
    if (idx_p3 == INFINITE) {
        idx_p3 = m_points.size();
        m_points.push_back(*p3);
    }
    if (n != NULL) {
        for(std::vector<Point>::iterator i=m_normals.begin(); i!=m_normals.end(); ++i)
        {
            if (i->x() == n->x() && i->y() == n->y() && i->z() == n->z()) {
                idx_n = i - m_normals.begin();
            }
        }
        if (idx_n == INFINITE) {
            idx_n = m_normals.size();
            m_normals.push_back(*n);
        }
    }
    size_t idx_e1=INFINITE;
    size_t idx_e2=INFINITE;
    size_t idx_e3=INFINITE;
    for(std::vector<Edge>::iterator i=m_edges.begin(); i!=m_edges.end(); ++i)
    {
        if (i->m_idx_p1 == idx_p1 && i->m_idx_p2 == idx_p2) {
            idx_e1 = i - m_edges.begin();
        }
        if (i->m_idx_p1 == idx_p2 && i->m_idx_p2 == idx_p1) {
            idx_e1 = i - m_edges.begin();
        }
        if (i->m_idx_p1 == idx_p2 && i->m_idx_p2 == idx_p3) {
            idx_e2 = i - m_edges.begin();
        }
        if (i->m_idx_p1 == idx_p3 && i->m_idx_p2 == idx_p2) {
            idx_e2 = i - m_edges.begin();
        }
        if (i->m_idx_p1 == idx_p3 && i->m_idx_p2 == idx_p1) {
            idx_e3 = i - m_edges.begin();
        }
        if (i->m_idx_p1 == idx_p1 && i->m_idx_p2 == idx_p3) {
            idx_e3 = i - m_edges.begin();
        }
    }
    if (idx_e1 == INFINITE) {
        idx_e1 = m_edges.size();
        m_edges.push_back(Edge(m_points, idx_p1, idx_p2));
    }
    if (idx_e2 == INFINITE) {
        idx_e2 = m_edges.size();
        m_edges.push_back(Edge(m_points, idx_p2, idx_p3));
    }
    if (idx_e3 == INFINITE) {
        idx_e3 = m_edges.size();
        m_edges.push_back(Edge(m_points, idx_p3, idx_p1));
    }
    m_triangles.push_back(Triangle(m_points, m_normals, m_edges, idx_p1, idx_p2, idx_p3, idx_e1, idx_e2, idx_e3, idx_n));
}

void 
Model::to_OpenGL()
{
    double normal_scale = 0.4;
#if 1
    glBegin(GL_TRIANGLES);
    for(std::vector<Triangle>::const_iterator i=m_triangles.begin(); i!=m_triangles.end(); ++i) {
        glNormal3f(i->n().x()*normal_scale,i->n().y()*normal_scale,i->n().z()*normal_scale);
        glVertex3f(i->p1().x(),i->p1().y(),i->p1().z());
        glVertex3f(i->p2().x(),i->p2().y(),i->p2().z());
        glVertex3f(i->p3().x(),i->p3().y(),i->p3().z());
    }
    glEnd();
#endif
#if 0
    glBegin(GL_LINES);
    for(std::vector<Triangle>::const_iterator i=m_triangles.begin(); i!=m_triangles.end(); ++i) {
        glVertex3f(i->e1().p1().x(),i->e1().p1().y(),i->e1().p1().z());
        glVertex3f(i->e1().p2().x(),i->e1().p2().y(),i->e1().p2().z());

        glVertex3f(i->e2().p1().x(),i->e2().p1().y(),i->e2().p1().z());
        glVertex3f(i->e2().p2().x(),i->e2().p2().y(),i->e2().p2().z());

        glVertex3f(i->e3().p1().x(),i->e3().p1().y(),i->e3().p1().z());
        glVertex3f(i->e3().p2().x(),i->e3().p2().y(),i->e3().p2().z());
    }
    glEnd();
#endif
}

void 
Model::n_OpenGL()
{
    double normal_scale = 0.3;
    glBegin(GL_LINES);
    for(std::vector<Triangle>::const_iterator i=m_triangles.begin(); i!=m_triangles.end(); ++i) {
        Point c = Point::mid(i->p1(), i->p2(), i->p3());
        Point cn = c.add(i->n().mul(normal_scale));
        glVertex3f(c.x(),c.y(),c.z());
        glVertex3f(cn.x(),cn.y(),cn.z());
    }
    glEnd();
}

void
Model::subdivide(double amount)
{
    std::vector<Triangle> new_triangles;
    for(std::vector<Triangle>::const_iterator i=m_triangles.begin(); i!=m_triangles.end(); ++i) {
        size_t idx_c = m_points.size();
        m_points.push_back(Point::mid(i->p1(),i->p2(),i->p3()).add(i->n().mul(amount)));
        size_t idx_e3 = m_edges.size();
        m_edges.push_back(Edge(m_points, i->m_idx_p1, idx_c));
        size_t idx_e4 = m_edges.size();
        m_edges.push_back(Edge(m_points, i->m_idx_p2, idx_c));
        size_t idx_e5 = m_edges.size();
        m_edges.push_back(Edge(m_points, i->m_idx_p3, idx_c));
        new_triangles.push_back(Triangle(m_points, m_normals, m_edges, i->m_idx_p1, i->m_idx_p2, idx_c, i->m_idx_e1, idx_e3, idx_e4));
        new_triangles.push_back(Triangle(m_points, m_normals, m_edges, i->m_idx_p2, i->m_idx_p3, idx_c, i->m_idx_e2, idx_e4, idx_e5));
        new_triangles.push_back(Triangle(m_points, m_normals, m_edges, i->m_idx_p3, i->m_idx_p1, idx_c, i->m_idx_e3, idx_e5, idx_e3));
    }
    m_triangles = new_triangles;
}

void
Model::resize(double min_x, double scale_x, double min_y, double scale_y, double min_z, double scale_z)
{
    for(std::vector<Point>::iterator i=m_points.begin(); i!=m_points.end(); ++i)
    {
        i->m_x = min_x + (i->x() - m_min_x) * scale_x;
        i->m_y = min_y + (i->y() - m_min_y) * scale_y;
        i->m_z = min_z + (i->z() - m_min_z) * scale_z;
    }
    if (scale_x != scale_y || scale_y != scale_z) {
        double n_x = 1/scale_x;
        double n_y = 1/scale_y;
        double n_z = 1/scale_z;
        double d = sqrt(sqr(n_x)+sqr(n_y)+sqr(n_z));
        n_x /= d;
        n_y /= d;
        n_z /= d;

        for(std::vector<Point>::iterator i=m_normals.begin(); i!=m_normals.end(); ++i)
        {
            i->m_x *= n_x;
            i->m_y *= n_y;
            i->m_z *= n_z;
        }
    }

    double _dim_x = dim_x();
    double _dim_y = dim_y();
    double _dim_z = dim_z();
    m_min_x = min_x;
    m_min_y = min_y;
    m_min_z = min_z;
    m_max_x = min_x + _dim_x*scale_x;
    m_max_y = min_y + _dim_y*scale_y;
    m_max_z = min_z + _dim_z*scale_z;
    for(std::vector<Triangle>::iterator i=m_triangles.begin(); i!=m_triangles.end(); ++i) {
        i->calculate_boundingbox();
    }
}

void
Model::rotate(double rot_x, double rot_y, double rot_z)
{
    QMatrix4x4 rot;
    rot.rotate(rot_x, 1, 0, 0);
    rot.rotate(rot_y, 0, 1, 0);
    rot.rotate(rot_z, 0, 0, 1);

    for(std::vector<Point>::iterator i=m_points.begin(); i!=m_points.end(); ++i)
    {
        QVector3D v = rot.map(QVector3D(i->x(), i->y(), i->z()));
        i->m_x = v.x();
        i->m_y = v.y();
        i->m_z = v.z();
    }
    for(std::vector<Point>::iterator i=m_normals.begin(); i!=m_normals.end(); ++i)
    {
        QVector3D v = rot.map(QVector3D(i->x(), i->y(), i->z()));
        i->m_x = v.x();
        i->m_y = v.y();
        i->m_z = v.z();
    }

    for(std::vector<Triangle>::iterator i=m_triangles.begin(); i!=m_triangles.end(); ++i) {
        i->calculate_boundingbox();
    }
    calculate_boundingbox();
}

void
Model::rotateip(double rot_x, double rot_y, double rot_z)
{
    QMatrix4x4 rot;
    rot.rotate(rot_x, 1, 0, 0);
    rot.rotate(rot_y, 0, 1, 0);
    rot.rotate(rot_z, 0, 0, 1);

    for(std::vector<Point>::iterator i=m_points.begin(); i!=m_points.end(); ++i)
    {
        QVector3D v = rot.map(QVector3D(i->x(), i->y(), i->z()));
        i->m_x = v.x();
        i->m_y = v.y();
        i->m_z = v.z();
    }
    for(std::vector<Point>::iterator i=m_normals.begin(); i!=m_normals.end(); ++i)
    {
        QVector3D v = rot.map(QVector3D(i->x(), i->y(), i->z()));
        i->m_x = v.x();
        i->m_y = v.y();
        i->m_z = v.z();
    }

    for(std::vector<Triangle>::iterator i=m_triangles.begin(); i!=m_triangles.end(); ++i) {
        i->calculate_boundingbox();
    }
    calculate_boundingbox();
}
