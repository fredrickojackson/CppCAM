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
#include "SimpleCutterDialog.h"

#include <cstdio>


/*SimpleCutterDialog::SimpleCutterDialog(QWidget* parent):QDialog(parent){
    setupUi(this);
}
*/




void SimpleCutterDialog::on_lineEdit_points_editingFinished()
{
    double res = 0.0;
    QString qs;
    if(radioButton_xdir->isChecked())
        res=(m_xmax-m_xmin)/(lineEdit_points->text().toInt()-1);
    else
        res=(m_ymax-m_ymin)/(lineEdit_points->text().toInt()-1);
    qs="resolution=";
    qs.append(QString::number(res)/*.append("\n")*/);
    textEditStatus->append(qs);
}


void SimpleCutterDialog::on_lineEdit_lines_editingFinished()
{
    double step = 0.0;
    QString qs;
    if(radioButton_xdir->isChecked())
        step=(m_ymax-m_ymin)/(lineEdit_lines->text().toInt()-1);
    else
        step=(m_xmax-m_xmin)/(lineEdit_lines->text().toInt()-1);
    qs="stepover=";
    qs.append(QString::number(step));
    textEditStatus->append(qs);
}

void SimpleCutterDialog::on_lineEdit_layers_editingFinished()
{
    double step = 0.0;
    QString qs;
    if(lineEdit_layers->text().toInt()==1){
        step=(m_zmax-m_zmin);
    }else{
        step=(m_zmax-m_zmin)/(lineEdit_layers->text().toInt()-1);
    }
    qs="stepdown=";
    qs.append(QString::number(step));
    textEditStatus->append(qs);
}
