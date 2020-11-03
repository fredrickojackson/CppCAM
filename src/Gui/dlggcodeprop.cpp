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

void dlgGcodeProp::on_pbToggle_clicked()
{
    if(ui->cbbeginrun->isChecked())
    {
        ui->cbbeginrun->setChecked(false);
        ui->cbbeginline->setChecked(false);
        ui->cbendrun->setChecked(false);
        ui->cbendline->setChecked(false);
        ui->cbslowline->setChecked(false);
    }else
    {
        ui->cbbeginrun->setChecked(true);
        ui->cbbeginline->setChecked(true);
        ui->cbendrun->setChecked(true);
        ui->cbendline->setChecked(true);
        ui->cbslowline->setChecked(true);
    }
}
