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
#include "ResizeModelDialog.h"

ResizeModelDialog::ResizeModelDialog(QWidget* parent):QDialog(parent)
{
    setupUi(this);
    connect(checkBox_scalemax,SIGNAL(toggled(bool)), this, SLOT(scalemax_toggled(bool)));
    connect(checkBox_scalex,SIGNAL(toggled(bool)), this, SLOT(scalexyz_toggled(bool)));
    connect(checkBox_scaley,SIGNAL(toggled(bool)), this, SLOT(scalexyz_toggled(bool)));
    connect(checkBox_scalez,SIGNAL(toggled(bool)), this, SLOT(scalexyz_toggled(bool)));
}

void
ResizeModelDialog::scalemax_toggled(bool checked)
{
    if (checked) {
        checkBox_scalex->setChecked(false);
        checkBox_scaley->setChecked(false);
        checkBox_scalez->setChecked(false);
    }
}

void 
ResizeModelDialog::scalexyz_toggled(bool checked)
{
    if (checked) {
        checkBox_scalemax->setChecked(false);
    }
}

