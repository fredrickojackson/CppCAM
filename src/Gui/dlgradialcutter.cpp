
#include "dlgradialcutter.h"

#include <cstdio>


dlgRadialCutter::dlgRadialCutter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgRadialCutter)
{
    ui->setupUi(this);
    flag_f=0.0;
    m_xmin=0.0;
    m_ymin=0.0;
    m_zmin=0.0;
    m_xmax=0.0;
    m_ymax=0.0;
    m_zmax=0.0;
    m_comp=0.0;
    m_smooth=0;
}

void dlgRadialCutter::showEvent(QShowEvent *event){
    if(m_comp){
        ui->cbLeaveMargin->setChecked(true);
        ui->leMargin->setText(QString::number(m_comp));
    }
    if(m_smooth){
        ui->leSmooth->setText(QString::number(m_smooth));
    }
    upddisp();
}


dlgRadialCutter::~dlgRadialCutter()
{
    delete ui;
}

void dlgRadialCutter::upddisp()
{
    double dres = 0.0;
    double dlin = 0.0;
    QString qs;
    if(ui->radioButton_xdir->isChecked())
    {
        dres=(m_xmax-m_xmin)/(ui->lineEdit_points->text().toInt()-1);
        dlin=(m_ymax-m_ymin)/(ui->lineEdit_lines->text().toInt()-1);
    }
    else
    {
        dres=(m_ymax-m_ymin)/(ui->lineEdit_points->text().toInt()-1);
        dlin=(m_xmax-m_xmin)/(ui->lineEdit_lines->text().toInt()-1);
    }
    qs.setNum(dres);
    ui->le_resolution->setText(qs);
    qs.setNum(dlin);
    ui->le_stepover->setText(qs);

    dres=(m_zmax-m_zmin)/(ui->lineEdit_layers->text().toInt()-1);
    qs.setNum(dres);
    ui->le_stepdown->setText(qs);
}

void dlgRadialCutter::on_le_stepover_editingFinished()
{
    QString s;
    if(ui->radioButton_xdir->isChecked())
        s.setNum((int)(1+(m_ymax-m_ymin)/(ui->le_stepover->text().toDouble())));
    if(ui->radioButton_ydir->isChecked())
        s.setNum((int)(1+(m_xmax-m_xmin)/(ui->le_stepover->text().toDouble())));
    ui->lineEdit_lines->setText(s);
    upddisp();
}

void dlgRadialCutter::on_le_resolution_editingFinished()
{
    QString s;
    if(ui->radioButton_xdir->isChecked())
        s.setNum((int)(1+(m_xmax-m_xmin)/(ui->le_resolution->text().toDouble())));
    if(ui->radioButton_ydir->isChecked())
        s.setNum((int)(1+(m_ymax-m_ymin)/(ui->le_resolution->text().toDouble())));
    ui->lineEdit_points->setText(s);
    upddisp();


}

void dlgRadialCutter::on_le_stepdown_editingFinished()
{
    QString s;
    s.setNum((int)(1+(m_zmax-m_zmin)/(ui->le_stepdown->text().toDouble())));
    ui->lineEdit_layers->setText(s);

}

void dlgRadialCutter::on_lineEdit_points_editingFinished()
{
    double res = 0.0;
    if(ui->radioButton_xdir->isChecked())
        res=(m_xmax-m_xmin)/(ui->lineEdit_points->text().toInt()-1);
    else
        res=(m_ymax-m_ymin)/(ui->lineEdit_points->text().toInt()-1);

    ui->le_resolution->setText(QString::number(res));
    upddisp();
}

void dlgRadialCutter::on_lineEdit_lines_editingFinished()
{
    double so = 0.0;
    if(ui->radioButton_xdir->isChecked())
        so=(m_ymax-m_ymin)/(ui->lineEdit_lines->text().toInt()-1);
    else
        so=(m_xmax-m_xmin)/(ui->lineEdit_lines->text().toInt()-1);

    ui->le_stepover->setText(QString::number(so));

    upddisp();
}

void dlgRadialCutter::on_lineEdit_layers_editingFinished()
{
    double stepdown = 0.0;
    stepdown=(m_zmax-m_zmin)/(ui->lineEdit_layers->text().toInt()-1);
    upddisp();
}

void dlgRadialCutter::on_leUseLine_textChanged(const QString/* &arg1*/)
{
    if(ui->leUseLine->text().toInt() == 1)ui->lblUseline->setText("Line");
    if(ui->leUseLine->text().toInt() == 2)ui->lblUseline->setText("nd Line");
    if(ui->leUseLine->text().toInt() == 3)ui->lblUseline->setText("rd Line");
    if(ui->leUseLine->text().toInt() >= 4)ui->lblUseline->setText("th Line");
}


void dlgRadialCutter::on_leMargin_editingFinished()
{
    if(ui->cbLeaveMargin->isChecked())
        m_comp=ui->leMargin->text().toDouble();
    else
        m_comp=0.0;
}

void dlgRadialCutter::on_cbLeaveMargin_stateChanged(int arg1)
{
    if(arg1)
        m_comp=ui->leMargin->text().toDouble();
    else
        m_comp=0.0;
}

void dlgRadialCutter::on_buttonBox_accepted()
{

}
