#include "dlgroundcutter.h"
#include "ui_dlgroundcutter.h"

dlgRoundCutter::dlgRoundCutter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgRoundCutter)
{
    ui->setupUi(this);
}

dlgRoundCutter::~dlgRoundCutter()
{
    delete ui;
}
