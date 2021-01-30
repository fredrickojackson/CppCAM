/* -*- coding: utf-8 -*-

Copyright 2008-2010 Lode Leroy
Copyright 2010 Lars Kruse <devel@sumpfralle.de>
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

#include "STLImporter.h"

#include <future>
#include <iostream>
#include <QFile>
#include <QRegExp>
#include <QDataStream>

class STLImportHelper {
    friend class STLImporter;
private:
    STLImportHelper(const std::string& filename) { this->filename = QString::fromStdString(filename); }
    QString filename;
    QFile f;
    unsigned numfacets;
    Model* ImportModel();
    Model* ImportTextModel();
    Model* ImportBinaryModel();
    Model* nImportBinaryModel();
};

namespace {
    static void log_error(QString msg) {
        std::cout << qPrintable(msg) << std::endl;
    }
    static void log_warn(QString msg) {
        std::cout << qPrintable(msg) << std::endl;
    }
    static void log_info(QString msg) {
        std::cout << qPrintable(msg) << std::endl;
    }
};

QString toString(const Point* p) { return QString("<%1,%2,%3>").arg(p->x()).arg(p->y()).arg(p->z()); }

Model* STLImporter::ImportModel(const std::string& filename)
{
    STLImportHelper importer(filename);
    return importer.ImportModel();
}

Model* STLImportHelper::ImportModel()
{
    f.setFileName(filename);
    if (!f.open(QIODevice::ReadOnly)) {
        log_error(QString("STLImporter: file not found: %1").arg(filename));
        return NULL;
    }
    if (f.size()>84) 
    {
        QByteArray buffer = f.read(84);
        numfacets = *(long*)buffer.mid(80,4).data();
        if (f.size() == 84 + 50*numfacets) {
            return nImportBinaryModel();
        }else{

        buffer.append(f.read(200-84));
        if (buffer.indexOf("solid") >= 0 && buffer.indexOf("facet") >= 0)
        {
            f.seek(0);
            return ImportTextModel();
        }
        
        log_error("STLImporter: STL binary/ascii detection failed");
    }
    return NULL;


  }
    return NULL;
}

// only works on big-endian machines, like intel... are there any others left?

float unpackFloat(const QByteArray& ba)
{
    return *(float*)ba.constData();
}

unsigned short unpackShort(const QByteArray& ba)
{
    return *(unsigned short*)ba.constData();
}

QString toString(const Point& p) { return QString("<%1,%2,%3>").arg(p.x()).arg(p.y()).arg(p.z()); }

Model* STLImportHelper::ImportBinaryModel()
{
    Model* model;
    model = new Model();
    bool normal_conflict_warning_seen = false;


    QDataStream data(&f);
    data.setByteOrder(QDataStream::LittleEndian);
    data.setFloatingPointPrecision(QDataStream::SinglePrecision);
    long it=sizeof(Point);
    it++;
    struct pp {
        float a1 = 0.0;
        float a2 = 0.0;
        float a3 = 0.0;

        float v11 = 0.0;
        float v12 = 0.0;
        float v13 = 0.0;

        float v21 = 0.0;
        float v22 = 0.0;
        float v23 = 0.0;

        float v31 = 0.0;
        float v32 = 0.0;
        float v33 = 0.0;

    };
    pp mypp;


    Point n(0.0, 0.0, 0.0);
    Point p1(0.0, 0.0, 0.0);
    Point p2(0.0, 0.0, 0.0);
    Point p3(0.0, 0.0, 0.0);
    f.seek(80);
    uint32_t tri_count;
    data >> tri_count;



    std::unique_ptr<uint8_t> buffer(new uint8_t[tri_count * 50]);
    data.readRawData((char*)buffer.get(), tri_count * 50);

    auto b = buffer.get();
    for(long i=0; i<numfacets; i++){
        memcpy(&mypp, b, 12*sizeof(float));
        b += 12*sizeof(float);
        b += sizeof(uint16_t);
        n.m_x=mypp.a1;
        n.m_y=mypp.a2;
        n.m_z=mypp.a3;
        p1.m_x=mypp.v11;
        p1.m_y=mypp.v12;
        p1.m_z=mypp.v13;
        p2.m_x=mypp.v21;
        p2.m_y=mypp.v22;
        p2.m_z=mypp.v23;
        p3.m_x=mypp.v31;
        p3.m_y=mypp.v32;
        p3.m_z=mypp.v33;


        double dotcross = n.dot(p2.sub(p1).cross(p3.sub(p1)));
        if (n.m_x == 0 && n.m_y == 0 && n.m_z == 0) {
            dotcross = p2.sub(p1).cross(p3.sub(p1)).z();
        }

        if (dotcross > 0) {
            //Triangle expects the vertices in clockwise order
            model->add_triangle(p1, p3, p2, n);
        } else if (dotcross < 0) {
            if (!normal_conflict_warning_seen) {
                log_warn(QString("Inconsistent normal/vertices found in facet "
                                 "definition %1 of '%2'. Please validate the "
                                 "STL file!").arg(i).arg(filename));
                normal_conflict_warning_seen = true;
            }
            model->add_triangle(p1, p2, p3, n);
        } else {
            // the three points are in a line - or two points are identical
            // usually this is caused by points, that are too close together
            // check the tolerance value in CppCAM/Geometry/PointKdtree.py
            log_warn(QString("Skipping invalid triangle: %1 / %2 / %3 ")
                     .arg(toString(p1)).arg(toString(p2)).arg(toString(p3))
                     + QString("maybe the resolution of the model is too high?)"));
        }


    }





    /*
    for(int i=0; i<numfacets; i++) {
        long fpos=f.pos();
        double a1 = unpackFloat(f.read(4));
        double a2 = unpackFloat(f.read(4));
        double a3 = unpackFloat(f.read(4));

        Point n(a1, a2, a3);

        double v11 = unpackFloat(f.read(4));
        double v12 = unpackFloat(f.read(4));
        double v13 = unpackFloat(f.read(4));

        Point p1(v11, v12, v13);

        double v21 = unpackFloat(f.read(4));
        double v22 = unpackFloat(f.read(4));
        double v23 = unpackFloat(f.read(4));

        Point p2(v21, v22, v23);

        double v31 = unpackFloat(f.read(4));
        double v32 = unpackFloat(f.read(4));
        double v33 = unpackFloat(f.read(4));

        Point p3(v31, v32, v33);

        // not used
        unsigned short attribs = unpackShort(f.read(2));
        // hmm... probably need to skip some bytes here...

        double dotcross = n.dot(p2.sub(p1).cross(p3.sub(p1)));
        if (a1 == 0 && a2 == 0 && a3 == 0) {
            dotcross = p2.sub(p1).cross(p3.sub(p1)).z();
        }

        if (dotcross > 0) {
            //Triangle expects the vertices in clockwise order
            model->add_triangle(p1, p3, p2, n);
        } else if (dotcross < 0) {
            if (!normal_conflict_warning_seen) {
                log_warn(QString("Inconsistent normal/vertices found in facet "
                                 "definition %1 of '%2'. Please validate the "
                                 "STL file!").arg(i).arg(filename));
                normal_conflict_warning_seen = true;
            }
            model->add_triangle(p1, p2, p3, n);
        } else {
            // the three points are in a line - or two points are identical
            // usually this is caused by points, that are too close together
            // check the tolerance value in CppCAM/Geometry/PointKdtree.py
            log_warn(QString("Skipping invalid triangle: %1 / %2 / %3 ")
                     .arg(toString(p1)).arg(toString(p2)).arg(toString(p3))
                     + QString("maybe the resolution of the model is too high?)"));
        }
        if((f.pos()-fpos)==0){
            break;
        }
    }
    */
    model->calculate_boundingbox();
    return model;
}

Model* STLImportHelper::nImportBinaryModel()
{
    Model* model;
    model = new Model();
    bool normal_conflict_warning_seen = false;

    for(unsigned int i=0; i<numfacets; i++) {
        long fpos=f.pos();
        double a1 = unpackFloat(f.read(4));
        double a2 = unpackFloat(f.read(4));
        double a3 = unpackFloat(f.read(4));

        Point n(a1, a2, a3);

        double v11 = unpackFloat(f.read(4));
        double v12 = unpackFloat(f.read(4));
        double v13 = unpackFloat( f.read(4));

        Point p1(v11, v12, v13);

        double v21 = unpackFloat(f.read(4));
        double v22 = unpackFloat(f.read(4));
        double v23 = unpackFloat(f.read(4));

        Point p2(v21, v22, v23);

        double v31 = unpackFloat(f.read(4));
        double v32 = unpackFloat(f.read(4));
        double v33 = unpackFloat(f.read(4));

        Point p3(v31, v32, v33);

        // not used
        unpackShort(f.read(2));
        // hmm... probably need to skip some bytes here...

        double dotcross = n.dot(p2.sub(p1).cross(p3.sub(p1)));
        if (a1 == 0 && a2 == 0 && a3 == 0) {
            dotcross = p2.sub(p1).cross(p3.sub(p1)).z();
        }

        if (dotcross > 0) {
            //Triangle expects the vertices in clockwise order
            model->add_triangle(p1, p3, p2, n);
        } else if (dotcross < 0) {
            if (!normal_conflict_warning_seen) {
                log_warn(QString("Inconsistent normal/vertices found in facet "
                                 "definition %1 of '%2'. Please validate the "
                                 "STL file!").arg(i).arg(filename));
                normal_conflict_warning_seen = true;
            }
            model->add_triangle(p1, p2, p3, n);
        } else {
            // the three points are in a line - or two points are identical
            // usually this is caused by points, that are too close together
            // check the tolerance value in CppCAM/Geometry/PointKdtree.py
            log_warn(QString("Skipping invalid triangle: %1 / %2 / %3 ")
                     .arg(toString(p1)).arg(toString(p2)).arg(toString(p3))
                     + QString("maybe the resolution of the model is too high?)"));
        }
        if((f.pos()-fpos)==0){
            break;
        }
    }
    model->calculate_boundingbox();
    return model;
}

Model* STLImportHelper::ImportTextModel()
{
    Model* model;

    QRegExp solid("\\s*solid\\s+(\\w+)\\s+.*");
    QRegExp endsolid("\\s*endsolid\\s*");
    QRegExp facet("\\s*facet\\s*");
    QRegExp normal("\\s*facet\\s+normal"
                   "\\s+((?:[-+]?\\d+(?:\\.\\d*)?|\\.\\d+)(?:[eE][-+]?\\d+)?)" 
                   "\\s+((?:[-+]?\\d+(?:\\.\\d*)?|\\.\\d+)(?:[eE][-+]?\\d+)?)" 
                   "\\s+((?:[-+]?\\d+(?:\\.\\d*)?|\\.\\d+)(?:[eE][-+]?\\d+)?)\\s+");
    QRegExp endfacet("\\s*endfacet\\s+");
    QRegExp loop("\\s*outer\\s+loop\\s+");
    QRegExp endloop("\\s*endloop\\s+");
    QRegExp vertex("\\s*vertex" 
                   "\\s+((?:[-+]?\\d+(?:\\.\\d*)?|\\.\\d+)(?:[eE][-+]?\\d+)?)" 
                   "\\s+((?:[-+]?\\d+(?:\\.\\d*)?|\\.\\d+)(?:[eE][-+]?\\d+)?)" 
                   "\\s+((?:[-+]?\\d+(?:\\.\\d*)?|\\.\\d+)(?:[eE][-+]?\\d+)?)\\s+");

    int current_line = 0;
        
    Point* n = NULL;
    Point* p1 = NULL;
    Point* p2 = NULL;
    Point* p3 = NULL;

    bool normal_conflict_warning_seen = false;

    model = new Model();

    while (!f.atEnd())
    {
        QString line = QString::fromUtf8(f.readLine());
        current_line++;
        if (solid.indexIn(line) == 0) {
            model->m_name = solid.cap(1).toStdString();
            continue;
        }
        if (facet.indexIn(line) == 0) {
            if (normal.indexIn(line) == 0) {
                n = new Point(normal.cap(1).toDouble(),normal.cap(2).toDouble(),normal.cap(3).toDouble());
            } 
            else
            {
                delete n;
                n = NULL;
            }
            continue;
        }
        if (loop.indexIn(line) == 0) {
            continue;
        }
        if (vertex.indexIn(line) == 0) {
            Point * p = new Point(vertex.cap(1).toDouble(),vertex.cap(2).toDouble(),vertex.cap(3).toDouble());
            if (p1 == NULL) 
                p1 = p;
            else if (p2 == NULL)
                p2 = p;
            else if (p3 == NULL)
                p3 = p;
            else
                log_error(QString("STLImporter: more then 3 points in facet "
                                  "(line %1)").arg(current_line));
            continue;
        }
        if (endloop.indexIn(line) == 0) {
            continue;
        }
        if (endfacet.indexIn(line) == 0) {
            if (p1 == NULL || p2 == NULL || p3 == NULL) {
                log_warn(QString("Invalid facet definition in line "
                                 "%1 of '%2'. Please validate the STL file!")
                         .arg(current_line).arg(f.fileName()));
                delete p1;
                delete p2;
                delete p3;
                delete n;
                n = p1 = p2 = p3 = NULL;
            }
            if (n == NULL) {
                n = new Point(p2->sub(p1).cross(p3->sub(p1)).normalized());
            }
            // validate the normal
            // The three vertices of a triangle in an STL file are supposed
            // to be in counter-clockwise order. This should match the
            // direction of the normal.
            double dotcross = 0;
            if (n != NULL) {
                // make sure the points are in ClockWise order
                dotcross = n->dot(p2->sub(p1).cross(p3->sub(p1)));
            }
            if (dotcross > 0) {
                // Triangle expects the vertices in clockwise order
                model->add_triangle(p1, p2, p3, n);
            }
            else if (dotcross < 0) {
                model->add_triangle(p1, p2, p3, n);
                if (!normal_conflict_warning_seen) {
                    log_warn(QString("Inconsistent normal/vertices found in "
                                     "line %1 of '%2'. Please validate the STL file!")
                             .arg(current_line).arg(f.fileName()));
                    normal_conflict_warning_seen = true;
                }
            }
            else {
                // The three points are in a line - or two points are
                // identical. Usually this is caused by points, that are too
                // close together. Check the tolerance value in
                // CppCAM/Geometry/PointKdtree.py.
                log_warn(QString("Skipping invalid triangle: %1 / %2 / %3 "
                                 "(maybe the resolution of the model is too high?)")
                         .arg(toString(p1)).arg(toString(p2)).arg(toString(p3)));
                delete p1;
                delete p2;
                delete p3;
                delete n;
                n = p1 = p2 = p3 = NULL;
                continue;
            }
            delete p1;
            delete p2;
            delete p3;
            delete n;
            n = p1 = p2 = p3 = NULL;
            continue;
        }
        if (endsolid.indexIn(line) == 0) {
            continue;
        }
    }
    log_info(QString("Imported STL model: %1 vertices, %2 edges, %3 triangles")
             .arg(model->m_points.size()).arg(model->m_edges.size()).arg(model->m_triangles.size()));

    delete p1;
    delete p2;
    delete p3;
    delete n;
    n = p1 = p2 = p3 = NULL;

    model->calculate_boundingbox();
    return model;
}
