#include "dlgtoolsize.h"
#include "ui_dlgtoolsize.h"

dlgToolSize::dlgToolSize(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgToolSize)
{
    ui->setupUi(this);
    ui->lineEditSize->setText(QString::number(m_cutterSize));
}

dlgToolSize::~dlgToolSize()
{
    m_cutterSize=ui->lineEditSize->text().toDouble();
    delete ui;
}

bool dlgToolSize::isSph()
{
    return ui->radioButtonSph->isChecked();
}

QString dlgToolSize::m_ts(){
    return ui->lineEditSize->text();
}

void dlgToolSize::on_lineEditSize_editingFinished()
{
    m_cutterSize=ui->lineEditSize->text().toDouble();
}

void dlgToolSize::showEvent(QShowEvent *event)
{
    if(m_cuttertype.compare("Sph")==0)
        ui->radioButtonSph->setChecked(true);
    else
        ui->radioButtonCyl->setChecked(true);
}
