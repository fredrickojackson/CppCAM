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
#include "PreferencesDialog.h"

#include <cstdio>

PreferencesDialog::PreferencesDialog(QWidget* parent):QDialog(parent)
{
    setupUi(this);
    m_gcodeSettings = NULL;
    m_gcodeIndex = -1;
    on_tabWidget_currentChanged(tabWidget->currentIndex());
}

void 
PreferencesDialog::on_buttonBox_accepted()
{
    if (m_gcodeSettings != NULL) {
        on_gcodeComboBox_currentIndexChanged(-1);
        m_gcodeSettings->sync();
    }
}

void 
PreferencesDialog::on_tabWidget_currentChanged(int index)
{
    if (index==1) {
        if (m_gcodeSettings == NULL) {
            m_gcodeSettings = new QSettings("/home/fred/GCode.conf", QSettings::IniFormat);
            QStringList headers = QStringList() << "header" << "preamble" << "postamble";
            Q_FOREACH(QString s, m_gcodeSettings->allKeys()) {
                if (!headers.contains(s)) {
                    headers << s;
                }
            }
            gcodeComboBox->addItems(headers);
        }
    }
}

void 
PreferencesDialog::on_gcodeComboBox_currentIndexChanged(int index)
{
    if (m_gcodeIndex >= 0) {
        m_gcodeSettings->setValue(gcodeComboBox->itemText(m_gcodeIndex), gcodeTextEdit->toPlainText());
    }
    if (index >= 0) {
        gcodeTextEdit->setText(m_gcodeSettings->value(gcodeComboBox->itemText(index)).toString());
        m_gcodeIndex = index;
    }
}

