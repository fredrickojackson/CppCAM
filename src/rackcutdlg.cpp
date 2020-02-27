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
