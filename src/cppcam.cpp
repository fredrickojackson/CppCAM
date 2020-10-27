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
#include "Stock.h"
#include "Model.h"
#include "Path.h"
#include "Cutter.h"
#include "HeightField.h"
#include "MainWindow.h"

#include <stdio.h>

extern int run_tests();


Stock* stock = NULL;
Model* model = NULL;
Cutter* cutter = NULL;
Cutter* cutter2 = NULL;
std::vector<Path*> paths;
HeightField* heightfield = NULL;

int main(int argc, char* argv[])
{
    if (argc>1 && strcmp(argv[1],"--test")==0) {
        return run_tests();
    }

    QApplication app(argc, argv);

    MainWindow w;
    w.commands=app.arguments();
    w.show();

    return app.exec();
}


