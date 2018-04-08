#include "dlgcutter.h"
#include "ui_dlgcutter.h"

dlgCutter::dlgCutter(QWidget *parent) :
    QDialog(parent), ui(new Ui::dlgCutter)
{
    ui->setupUi(this);
}

dlgCutter::~dlgCutter()
{
    delete ui;
}

void dlgCutter::on_buttonBox_accepted()
{
    this->close();
}
