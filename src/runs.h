#pragma once
#include "Cutter.h"
#include "Model.h"
#include "Stock.h"
#include "Geometry/Point.h"


struct r_run
{
    int type;    //0=x 1=y 2=rect 3=xrad 4=yrad 5-rectrad
//    Model* model;
    Cutter* cutter;
    Stock* stock;
    int lines;
    int points;
    int layers;
    double safez;
    double fspd;
    double pspd;
    double margin;
    double rotatetotal;
    double rotatestep;

    int smooth;
    Point dir;
    int r_useLine;

};
