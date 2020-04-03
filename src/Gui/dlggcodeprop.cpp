#include "dlggcodeprop.h"
#include "ui_dlggcodeprop.h"

dlgGcodeProp::dlgGcodeProp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgGcodeProp)
{
    ui->setupUi(this);

}

dlgGcodeProp::~dlgGcodeProp()
{
    delete ui;
}
