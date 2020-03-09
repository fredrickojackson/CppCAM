#include "rackcutdlg.h"
#include "ui_rackcutdlg.h"

rackcutdlg::rackcutdlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::rackcutdlg)
{
    ui->setupUi(this);
}

rackcutdlg::~rackcutdlg()
{
    delete ui;
}

void rackcutdlg::on_cbClear_clicked()
{
    if(ui->cbClear->isChecked())
    {
        ui->dsbStepDown->setEnabled(true);
        ui->dsbStepOver->setEnabled(true);
    }else
    {
        ui->dsbStepDown->setEnabled(false);
        ui->dsbStepOver->setEnabled(false);
    }
}

void rackcutdlg::on_cbClear_stateChanged(int arg1)
{

}
