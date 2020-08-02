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



SimpleCutterDialog::SimpleCutterDialog(QWidget* parent){
    setupUi(this);
    lineEdit_lines->installEventFilter(this);
    lineEdit_points->installEventFilter(this);
    flag_f=0.0;
    m_xmin=0.0;
    m_ymin=0.0;
    m_zmin=0.0;
    m_xmax=0.0;
    m_ymax=0.0;
    m_zmax=0.0;
    m_comp=0.0;
    m_smooth=0;
}

bool SimpleCutterDialog::eventFilter(QObject *object, QEvent *event)
{
    double res = 0.0;
    double step = 0.0;
    if (event->type() == QEvent::FocusOut)
    {
        if (object == lineEdit_lines || object == lineEdit_points)
        {
            if(radioButton_xdir->isChecked() || rbrect->isChecked())
            {
                res=(m_xmax-m_xmin)/(lineEdit_points->text().toInt()-1);
                step=(m_ymax-m_ymin)/(lineEdit_lines->text().toInt()-1);

            }
            if(radioButton_ydir->isChecked())
            {
                res=(m_ymax-m_ymin)/(lineEdit_points->text().toInt()-1);
                step=(m_xmax-m_xmin)/(lineEdit_lines->text().toInt()-1);
            }
            le_resolution->setText(QString::number(res));
            le_stepover->setText(QString::number(step));
        }
    }

    return false;
}



void SimpleCutterDialog::showEvent(QShowEvent *event){
    if(m_comp){
        cbLeaveMargin->setChecked(true);
        leMargin->setText(QString::number(m_comp));
    }
    if(m_smooth){
        leSmooth->setText(QString::number(m_smooth));
    }

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
    le_stepdown->setText(QString::number(step));
}


void SimpleCutterDialog::on_leUseLine_textChanged(const QString &arg1)
{
    if(leUseLine->text().toInt() == 1)lblUseline->setText("Line");
    if(leUseLine->text().toInt() == 2)lblUseline->setText("nd Line");
    if(leUseLine->text().toInt() == 3)lblUseline->setText("rd Line");
    if(leUseLine->text().toInt() >= 4)lblUseline->setText("th Line");
}

void SimpleCutterDialog::on_leMargin_editingFinished()
{
    if(cbLeaveMargin->isChecked())
        m_comp=leMargin->text().toDouble();
    else
        m_comp=0.0;
}

void SimpleCutterDialog::on_cbLeaveMargin_stateChanged(int arg1)
{
    if(arg1)
        m_comp=leMargin->text().toDouble();
    else
        m_comp=0.0;
}

void SimpleCutterDialog::on_buttonBox_accepted()
{

}
