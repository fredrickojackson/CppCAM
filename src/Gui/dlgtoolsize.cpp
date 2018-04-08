#include "dlgtoolsize.h"
#include "ui_dlgtoolsize.h"

dlgToolSize::dlgToolSize(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgToolSize)
{
    ui->setupUi(this);
}

dlgToolSize::~dlgToolSize()
{
    delete ui;
}

bool dlgToolSize::isSph()
{
    return ui->radioButtonSph->isChecked();
}

QString dlgToolSize::m_ts(){
    return ui->lineEditSize->text();
}
