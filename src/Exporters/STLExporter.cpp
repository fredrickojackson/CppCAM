/* -*- coding: utf-8 -*-

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

#include "STLExporter.h"

#include <iostream>
#include <QFile>
#include <QRegExp>

class STLExportHelper {
    friend class STLExporter;
private:
    STLExportHelper() { }
    bool ExportTextModel(const Model* model, const QString& filename);
    bool ExportBinaryModel(const Model* model, const QString& filename);
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

bool STLExporter::ExportModel(const Model* model, const std::string filename, bool binary)
{
    STLExportHelper exporter;
    if (binary) {
        return exporter.ExportBinaryModel(model, QString::fromStdString(filename));
    } else {
        return exporter.ExportTextModel(model, QString::fromStdString(filename));
    }
}

// only works on big-endian machines, like intel... are there any others left?
QByteArray packFloat(float value)
{
    return QByteArray((const char*)&value, 4);
}

QByteArray packShort(unsigned short value)
{
    return QByteArray((const char*)&value, 2);
}

QByteArray packLong(unsigned long value)
{
    return QByteArray((const char*)&value, 4);
}

bool STLExportHelper::ExportBinaryModel(const Model* model, const QString& filename)
{
    QFile f;
    f.setFileName(filename);
    if (!f.open(QIODevice::WriteOnly)) {
        log_error(QString("STLImporter: file not found: %1").arg(filename));
        return false;
    }
    f.write(QString("%1").arg(QString::fromStdString(model->m_name), 80, ' ').toLatin1());
    f.write(packLong(model->m_triangles.size()));
    for(std::vector<Triangle>::const_iterator t = model->m_triangles.begin(); t != model->m_triangles.end(); t++) {
        f.write(packFloat(t->n().x()));
        f.write(packFloat(t->n().y()));
        f.write(packFloat(t->n().z()));

        f.write(packFloat(t->p1().x()));
        f.write(packFloat(t->p1().y()));
        f.write(packFloat(t->p1().z()));

        f.write(packFloat(t->p2().x()));
        f.write(packFloat(t->p2().y()));
        f.write(packFloat(t->p2().z()));

        f.write(packFloat(t->p3().x()));
        f.write(packFloat(t->p3().y()));
        f.write(packFloat(t->p3().z()));

        f.write(packShort(0));
    }
    f.close();
    return true;
}

bool STLExportHelper::ExportTextModel(const Model* model, const QString& filename)
{
    QFile f;
    f.setFileName(filename);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        log_error(QString("STLImporter: file not found: %1").arg(filename));
        return false;
    }

    f.write(QString("solid %1\n").arg(QString::fromStdString(model->m_name)).toUtf8());
    for(std::vector<Triangle>::const_iterator t = model->m_triangles.begin(); t != model->m_triangles.end(); t++) {
        f.write(QString("  facet normal %1 %2 %3\n").arg(QString::number(t->n().x())).arg(QString::number(t->n().y())).arg(QString::number(t->n().z())).toUtf8());
        f.write(QString("    outer loop\n").toUtf8());
        f.write(QString("      vertex %1 %2 %3\n").arg(QString::number(t->p1().x())).arg(QString::number(t->p1().y())).arg(QString::number(t->p1().z())).toUtf8());
        f.write(QString("      vertex %1 %2 %3\n").arg(QString::number(t->p2().x())).arg(QString::number(t->p2().y())).arg(QString::number(t->p2().z())).toUtf8());
        f.write(QString("      vertex %1 %2 %3\n").arg(QString::number(t->p3().x())).arg(QString::number(t->p3().y())).arg(QString::number(t->p3().z())).toUtf8());
        f.write(QString("    endloop\n").toUtf8());
        f.write(QString("  endfacet\n").toUtf8());
    }
    f.write(QString("endsolid %1\n").arg(QString::fromStdString(model->m_name)).toUtf8());
    log_info(QString("Exported STL model: %1 vertices, %2 edges, %3 triangles")
             .arg(model->m_points.size()).arg(model->m_edges.size()).arg(model->m_triangles.size()));
    f.close();
    return true;
}
