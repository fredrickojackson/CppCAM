/* -*- coding: utf-8 -*-

Copyright 2014 Lode Leroy

This file is part of PyCAM.

PyCAM is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

PyCAM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with PyCAM.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef __DROPCUTTER_H
#define __DROPCUTTER_H

#include "Cutter.h"
#include "Model.h"
#include "HeightField.h"

class DropCutterImpl;

class DropCutter {
public:
    DropCutter();
    ~DropCutter();
    void GeneratePath(const Cutter& cutter, const Model& model, HeightField& heightfield);
    void setNumthreads(size_t numthreads);
    void setAsync(bool async);
    bool finished(double* progress=NULL);
private:
    DropCutterImpl *m_impl;
    bool m_async;
};

#endif
