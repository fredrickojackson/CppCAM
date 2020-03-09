#include "dlgholecut.h"
#include "ui_dlgholecut.h"

dlgHoleCut::dlgHoleCut(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgHoleCut)
{
    ui->setupUi(this);
}

dlgHoleCut::~dlgHoleCut()
{
    delete ui;
}

void dlgHoleCut::on_pbDone_clicked()
{
    myresult=0;
    this->close();
}

void dlgHoleCut::on_pbApply_clicked()
{
    myresult=1;
    this->close();
}
